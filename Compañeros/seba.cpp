#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <map>
#include <omp.h>
#include <algorithm>
#include <queue> 
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;

const int TIME_LIMIT_SEC = 20;

// Hash para pair<int,int> (IDs)
struct pair_int_hash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

// Convierte string a ID numérico único con diccionario compartido
int obtenerID(const string& cp, unordered_map<string, int>& dicc, mutex& mtx, int& id_counter) {
    lock_guard<mutex> lock(mtx);
    auto it = dicc.find(cp);
    if (it != dicc.end()) return it->second;
    int nuevo_id = id_counter++;
    dicc[cp] = nuevo_id;
    return nuevo_id;
}

// Parsea línea y extrae CP origen/destino
bool parsearLinea(const char* start, size_t len, string& cp_origen, string& cp_destino) {
    const char* cur = start;
    const char* end = start + len;

    for (int i = 0; i < 6; ++i) {
        while (cur < end && *cur != '"') ++cur;
        ++cur;
        while (cur < end && *cur != '"') ++cur;
        ++cur;
        if (cur < end && *cur == ';') ++cur;
    }

    // Origen
    if (cur < end && *cur == '"') ++cur;
    const char* ini_o = cur;
    while (cur < end && *cur != '"') ++cur;
    cp_origen = string(ini_o, cur - ini_o);
    ++cur; if (cur < end && *cur == ';') ++cur;

    // Destino
    if (cur < end && *cur == '"') ++cur;
    const char* ini_d = cur;
    while (cur < end && *cur != '"') ++cur;
    cp_destino = string(ini_d, cur - ini_d);

    return !cp_origen.empty() && !cp_destino.empty();
}

int main() {
    auto t0 = chrono::steady_clock::now();
    string ruta = "C:/Users/sebal/Downloads/eldoria.csv";

    const char* data = nullptr;
    size_t file_size = 0;

#ifdef _WIN32
    HANDLE hFile = CreateFileA(ruta.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    LARGE_INTEGER li;
    GetFileSizeEx(hFile, &li);
    file_size = static_cast<size_t>(li.QuadPart);
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    data = static_cast<const char*>(MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0));
#else
    int fd = open(ruta.c_str(), O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);
    file_size = sb.st_size;
    data = static_cast<const char*>(mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    close(fd);
#endif

    if (!data || file_size == 0) {
        cerr << "Archivo inválido.\n";
        return 1;
    }

    const int N_THREADS = omp_get_max_threads();
    size_t bloque = file_size / N_THREADS;

    // Diccionario compartido para convertir CPs en IDs numéricos
    unordered_map<string, int> diccionario;
    mutex mtx_diccionario;
    int id_counter = 0;

    // Cada hilo almacena pares (ID_origen, ID_destino) y cantidad
    vector<vector<pair<pair<int, int>, int>>> datos_locales(N_THREADS);
    vector<mutex> mutexes(N_THREADS);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        size_t ini = tid * bloque;
        size_t fin = (tid == N_THREADS - 1) ? file_size : (tid + 1) * bloque;

        const char* ptr = data + ini;
        const char* end = data + fin;

        // Saltar header
        if (tid == 0) {
            while (*ptr != '\n') ++ptr;
            ++ptr;
        } else {
            while (ptr < end && *ptr != '\n') ++ptr;
            ++ptr;
        }

        while (ptr < end && chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - t0).count() < TIME_LIMIT_SEC) {
            const char* salto = find(ptr, end, '\n');
            const char* fin_linea = (salto == end) ? end : salto;

            string origen, destino;
            if (parsearLinea(ptr, fin_linea - ptr, origen, destino)) {
                int id_origen = obtenerID(origen, diccionario, mtx_diccionario, id_counter);
                int id_destino = obtenerID(destino, diccionario, mtx_diccionario, id_counter);

                // Almacena en local
                datos_locales[tid].push_back({{id_origen, id_destino}, 1});
            }

            ptr = salto + 1;
        }
    }

#ifdef _WIN32
    UnmapViewOfFile(data);
    CloseHandle(hMap);
    CloseHandle(hFile);
#else
    munmap((void*)data, file_size);
#endif

    // Combinar todos los conteos
    unordered_map<pair<int, int>, int, pair_int_hash> global;
    for (const auto& local : datos_locales) {
        for (const auto& par : local) {
            global[par.first] += par.second;
        }
    }

    // Invertir diccionario para recuperar CPs
    vector<string> id_to_cp(id_counter);
    for (const auto& p : diccionario) {
        id_to_cp[p.second] = p.first;
    }

    // Obtener top 10.000
    using Registro = pair<pair<int, int>, int>;
    auto cmp = [](const Registro& a, const Registro& b) { return a.second > b.second; };
    priority_queue<Registro, vector<Registro>, decltype(cmp)> heap(cmp);

    for (const auto& p : global) {
        heap.push(p);
        if (heap.size() > 10000) heap.pop();
    }

    vector<Registro> top;
    while (!heap.empty()) {
        top.push_back(heap.top());
        heap.pop();
    }
    reverse(top.begin(), top.end());

    // Guardar en CSV
    ofstream out("top_10000_trayectos.csv");
    out << "CP_ORIGEN;CP_DESTINO;VIAJES\n";
    for (const auto& r : top) {
        out << id_to_cp[r.first.first] << ";" << id_to_cp[r.first.second] << ";" << r.second << "\n";
    }
    out.close();

    auto t1 = chrono::steady_clock::now();
    auto dur = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();
    cout << "Duración total: " << dur << " ms (" << dur / 1000.0 << " segundos)\n";
    cout << "Archivo generado: top_10000_trayectos.csv\n";

    return 0;
}
