#include <iostream> // Entrada/salida estándar
#include <string> // Manejo de strings
#include <vector> // Vectores dinámicos
#include <fstream> // Archivos de texto
#include <chrono> // Medición de tiempo
#include <mutex> // Exclusión mutua para hilos
#include <unordered_map> // Diccionario hash
#include <map> // Diccionario ordenado (no usado aquí)
#include <omp.h> // OpenMP para paralelismo
#include <algorithm> // Algoritmos estándar (find, reverse, etc)
#include <queue> // Cola de prioridad
#ifdef _WIN32 // Inclusión de headers para Windows
#include <windows.h> // API de Windows para mapeo de archivos
#else // Inclusión de headers para Linux/Unix
#include <sys/mman.h> // Mapeo de archivos en memoria
#include <sys/stat.h> // Información de archivos
#include <fcntl.h> // Control de archivos
#include <unistd.h> // Funciones POSIX
#endif

//g++ -std=c++17 -fopenmp "parte4.5.cpp" -o trayectos 
//trayectos



using namespace std; // Uso del espacio de nombres estándar

const int TIME_LIMIT_SEC = 20; // Límite de tiempo de procesamiento (segundos)

// Hash para pair<int,int> (IDs), necesario para usar como clave en unordered_map
struct pair_int_hash {
    size_t operator()(const pair<int, int>& p) const {
        // Combina los hashes de los dos enteros
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

// Convierte un string (código postal) a un ID numérico único usando un diccionario compartido entre hilos
int obtenerID(const string& cp, unordered_map<string, int>& dicc, mutex& mtx, int& id_counter) {
    lock_guard<mutex> lock(mtx); // Bloquea el acceso al diccionario
    auto it = dicc.find(cp); // Busca si ya existe el CP
    if (it != dicc.end()) return it->second; // Si existe, retorna el ID
    int nuevo_id = id_counter++; // Si no, asigna un nuevo ID
    dicc[cp] = nuevo_id; // Lo guarda en el diccionario
    return nuevo_id;
}

// Parsea una línea del CSV y extrae los códigos postales de origen y destino
bool parsearLinea(const char* start, size_t len, string& cp_origen, string& cp_destino) {
    const char* cur = start;
    const char* end = start + len;

    // Salta los primeros 6 campos (no usados)
    for (int i = 0; i < 6; ++i) {
        while (cur < end && *cur != '"') ++cur; // Busca comillas de apertura
        ++cur;
        while (cur < end && *cur != '"') ++cur; // Busca comillas de cierre
        ++cur;
        if (cur < end && *cur == ';') ++cur; // Salta punto y coma
    }

    // Extrae CP origen
    if (cur < end && *cur == '"') ++cur;
    const char* ini_o = cur;
    while (cur < end && *cur != '"') ++cur;
    cp_origen = string(ini_o, cur - ini_o);
    ++cur; if (cur < end && *cur == ';') ++cur;

    // Extrae CP destino
    if (cur < end && *cur == '"') ++cur;
    const char* ini_d = cur;
    while (cur < end && *cur != '"') ++cur;
    cp_destino = string(ini_d, cur - ini_d);

    // Devuelve true si ambos CPs no están vacíos
    return !cp_origen.empty() && !cp_destino.empty();
}

int main() {
    auto t0 = chrono::steady_clock::now(); // Marca el inicio para medir tiempo
    string ruta = "E:/Proyectos/ProyectoParalelo/datos_prueba.csv"; // Ruta del archivo CSV

    const char* data = nullptr; // Puntero a los datos mapeados
    size_t file_size = 0; // Tamaño del archivo

#ifdef _WIN32
    // Mapea el archivo en memoria (Windows)
    HANDLE hFile = CreateFileA(ruta.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    LARGE_INTEGER li;
    GetFileSizeEx(hFile, &li);
    file_size = static_cast<size_t>(li.QuadPart);
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    data = static_cast<const char*>(MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0));
#else
    // Mapea el archivo en memoria (Linux/Unix)
    int fd = open(ruta.c_str(), O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);
    file_size = sb.st_size;
    data = static_cast<const char*>(mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    close(fd);
#endif

    // Verifica que el archivo se haya leído correctamente
    if (!data || file_size == 0) {
        cerr << "Archivo inválido.\n";
        return 1;
    }

    const int N_THREADS = omp_get_max_threads(); // Número de hilos disponibles
    size_t bloque = file_size / N_THREADS; // Tamaño de bloque por hilo

    // Diccionario compartido para convertir CPs en IDs numéricos
    unordered_map<string, int> diccionario;
    mutex mtx_diccionario; // Mutex para acceso seguro al diccionario
    int id_counter = 0; // Contador de IDs únicos

    // Cada hilo almacena pares (ID_origen, ID_destino) y cantidad
    vector<vector<pair<pair<int, int>, int>>> datos_locales(N_THREADS); // Resultados locales por hilo
    vector<mutex> mutexes(N_THREADS); // (No usado, pero reservado por si se requiere)

    // Procesamiento paralelo de las líneas del archivo
    #pragma omp parallel
    {
        int tid = omp_get_thread_num(); // ID del hilo
        size_t ini = tid * bloque; // Inicio del bloque
        size_t fin = (tid == N_THREADS - 1) ? file_size : (tid + 1) * bloque; // Fin del bloque

        const char* ptr = data + ini; // Puntero de inicio
        const char* end = data + fin; // Puntero de fin

        // Salta el header (primera línea)
        if (tid == 0) {
            while (*ptr != '\n') ++ptr;
            ++ptr;
        } else {
            while (ptr < end && *ptr != '\n') ++ptr;
            ++ptr;
        }

        // Procesa cada línea dentro del bloque y dentro del límite de tiempo
        while (ptr < end && chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - t0).count() < TIME_LIMIT_SEC) {
            const char* salto = find(ptr, end, '\n'); // Busca fin de línea
            const char* fin_linea = (salto == end) ? end : salto;

            string origen, destino;
            if (parsearLinea(ptr, fin_linea - ptr, origen, destino)) {
                // Convierte los CPs a IDs numéricos
                int id_origen = obtenerID(origen, diccionario, mtx_diccionario, id_counter);
                int id_destino = obtenerID(destino, diccionario, mtx_diccionario, id_counter);

                // Almacena el par (origen, destino) en los datos locales del hilo
                datos_locales[tid].push_back({{id_origen, id_destino}, 1});
            }

            ptr = salto + 1; // Avanza a la siguiente línea
        }
    }


#ifdef _WIN32
    // Libera recursos de mapeo de memoria (Windows)
    UnmapViewOfFile(data);
    CloseHandle(hMap);
    CloseHandle(hFile);
#else
    // Libera recursos de mapeo de memoria (Linux/Unix)
    munmap((void*)data, file_size);
#endif


    // Combina los conteos de todos los hilos en un solo diccionario global
    unordered_map<pair<int, int>, int, pair_int_hash> global;
    for (const auto& local : datos_locales) {
        for (const auto& par : local) {
            global[par.first] += par.second; // Suma la cantidad de viajes para cada par (origen, destino)
        }
    }


    // Invierte el diccionario para poder recuperar el CP a partir del ID
    vector<string> id_to_cp(id_counter);
    for (const auto& p : diccionario) {
        id_to_cp[p.second] = p.first;
    }


    // Obtiene los 10.000 pares (origen, destino) con más viajes usando un heap mínimo
    using Registro = pair<pair<int, int>, int>;
    auto cmp = [](const Registro& a, const Registro& b) { return a.second > b.second; };
    priority_queue<Registro, vector<Registro>, decltype(cmp)> heap(cmp);

    for (const auto& p : global) {
        heap.push(p); // Agrega el par al heap
        if (heap.size() > 10000) heap.pop(); // Mantiene solo los 10.000 mayores
    }

    vector<Registro> top;
    while (!heap.empty()) {
        top.push_back(heap.top());
        heap.pop();
    }
    reverse(top.begin(), top.end()); // Ordena de mayor a menor


    // Guarda el resultado en un archivo CSV
    ofstream out("top_10000_trayectos.csv");
    out << "CP_ORIGEN;CP_DESTINO;VIAJES\n";
    for (const auto& r : top) {
        out << id_to_cp[r.first.first] << ";" << id_to_cp[r.first.second] << ";" << r.second << "\n";
    }
    out.close();

    auto t1 = chrono::steady_clock::now(); // Marca el fin
    auto dur = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count(); // Calcula duración
    cout << "Duración total: " << dur << " ms (" << dur / 1000.0 << " segundos)\n";
    cout << "Archivo generado: top_10000_trayectos.csv\n";

    return 0;
}
