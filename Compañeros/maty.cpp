#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>
#include <map>
#include <mutex>

using namespace std;

// Estructura para almacenar cada registro del CSV
/**
struct Registro {
    string identificador;
    string especie;
    string genero;
    string nombre;
    string apellido;
    string fechaNacimiento;
    string cpOrigen;
    string cpDestino;
};
*/

struct Estadisticas
{
    vector<int> Estrato={0,0,0,0,0,0,0,0,0};
    int total=0;
    map<pair<string,string>,vector<int>> Edades;
};

mutex Bloqueo;

chrono::system_clock::time_point tm_to_time_point(const std::tm& t) {
    // mktime espera tiempo local, pero si la cadena es UTC, puede haber un desfase.
    // Para simplicidad, asumimos que la cadena es en la misma zona horaria que el sistema.
    std::time_t tt = std::mktime(const_cast<std::tm*>(&t));
    return std::chrono::system_clock::from_time_t(tt);
}


// Obtiene el tamaño en bytes de un archivo dado su ruta
size_t obtenerTamanoArchivo(const string& rutaArchivo) {
    ifstream archivo(rutaArchivo, ios::binary | ios::ate);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return 0;
    }
    return static_cast<size_t>(archivo.tellg());
}

// Devuelve la cantidad de hilos disponibles en el sistema
unsigned int obtenerCantidadHilos() {
    unsigned int n = thread::hardware_concurrency();
    return (n == 0) ? 2 : n; // Si no se detecta, usar 2 por defecto
}

// Calcula el tamaño de cada bloque para la lectura paralela
size_t calcularTamanoBloque(size_t tamanoArchivo, unsigned int numHilos) {
    return tamanoArchivo / numHilos;
}
/** 
 * Parsea una línea del CSV y la convierte en un struct Registro.
 * Se asume que los campos están entre comillas y separados por ';'
 * Ejemplo de línea: "123";"Perro";"Macho";"Fido";"Gomez";"2010-01-01";"1000";"2000"
*/
void parsearLineaCSV(const string& linea, Estadisticas& Resultado) {

    size_t inicio = 0, fin = 0;
    string campos[8];
    int campoActual = 0;

    while (campoActual < 8 && inicio < linea.size()) {
        // Buscar la primera comilla
        inicio = linea.find('\"', fin);
        if (inicio == string::npos) break;
        // Buscar la segunda comilla
        fin = linea.find('\"', inicio + 1);
        if (fin == string::npos) break;
        // Extraer el campo entre comillas
        campos[campoActual++] = linea.substr(inicio + 1, fin - inicio - 1);
        // Avanzar al siguiente separador
        fin = linea.find(';', fin);
        if (fin == string::npos) fin = linea.size();
    }

    lock_guard<mutex> lock(Bloqueo);
    Resultado.Estrato[int(campos[6][0])]+=1;
    //Resultado.Edades[{campos[1],campos[2]}].push_back()
    // Asignar los campos extraídos al struct Registro
    /*campos[0];
    campos[1];
    campos[2];
    campos[3];
    campos[4];
    campos[5];
    campos[6];
    campos[7];*/

}
/** 
 * Lee y procesa un bloque del archivo CSV.
 * - rutaArchivo: ruta al archivo CSV.
 * - inicio: posición inicial (en bytes) del bloque.
 * - fin: posición final (en bytes) del bloque.
 * - registros: vector donde se guardarán los registros leídos.
 *
 * Esta función:
 * 1. Abre el archivo y se posiciona en 'inicio'.
 * 2. Si 'inicio' no es 0, avanza hasta el próximo salto de línea para no cortar una línea.
 * 3. Lee línea por línea hasta llegar a 'fin' o al final del archivo.
 * 4. Parsea cada línea y la guarda en 'registros'.
 */
void procesarBloque(const string& rutaArchivo, size_t inicio, size_t fin, Estadisticas& Resultado, bool esPrimerHilo) {
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo en el hilo." << endl;
        return;
    }

    archivo.seekg(inicio);

    // Si es el primer hilo, saltar encabezado
    if (esPrimerHilo) {
        string encabezado;
        getline(archivo, encabezado);
    } else if (inicio != 0) {
        // Retrocede un byte para ver si está justo después de un salto de línea
        archivo.seekg(inicio - 1);
        char c;
        archivo.get(c);
        if (c != '\n' && c != '\r') {
            // Si no está al inicio de línea, descarta la línea incompleta
            string dummy;
            getline(archivo, dummy);
        }
    }

    string linea;
    size_t posActual = archivo.tellg();

    while (posActual < fin && getline(archivo, linea)) {
        if (linea.empty()) continue;
        parsearLineaCSV(linea,ref(Resultado));
        posActual = archivo.tellg();
        if (archivo.eof()) break;
    }
}

int main(){
   std::string fecha_dada_str = "1936-04-09T03:10:06";
   fecha_dada_str[fecha_dada_str.find('T')]=' ';

    // 1. Parsear la fecha y hora dada
    std::tm tm_dada = {}; // Inicializar a ceros
    std::stringstream ss(fecha_dada_str);

    ss >> std::get_time(&tm_dada, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        std::cerr << "Error al parsear la fecha y hora: " << fecha_dada_str << std::endl;
        return 1;
    }

    // ***** LA CORRECCIÓN CLAVE AQUÍ *****
    // std::tm::tm_year guarda años desde 1900.
    // Aunque %Y lee el año completo, std::get_time a veces lo deja así en tm_year.
    // Lo ajustamos para que mktime lo interprete correctamente.
    // Por ejemplo, si tm_dada.tm_year es 1936, ahora será 36.
    tm_dada.tm_year -= 1900;
    // *************************************


    // Convertir std::tm a std::chrono::system_clock::time_point
    std::chrono::system_clock::time_point tiempo_dado;
    try {
        tiempo_dado = tm_to_time_point(tm_dada);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    // 2. Obtener la fecha y hora actual
    std::chrono::system_clock::time_point tiempo_actual = std::chrono::system_clock::now();

    // 3. Calcular la diferencia
    std::chrono::duration<double> diferencia_segundos = tiempo_actual - tiempo_dado;

    // Convertir la diferencia a unidades más legibles
    long long total_segundos = static_cast<long long>(diferencia_segundos.count());

    long long segundos = total_segundos % 60;
    long long minutos = (total_segundos / 60) % 60;
    long long horas = (total_segundos / (60 * 60)) % 24;
    long long dias = total_segundos / (60 * 60 * 24);

    std::cout << "Fecha y hora dada: " << fecha_dada_str << std::endl;

    // Imprimir la fecha y hora actual de forma legible
    std::time_t t_actual = std::chrono::system_clock::to_time_t(tiempo_actual);
    // std::localtime no es thread-safe, usar std::localtime_s o un mutex si es en multithreading
    // Para este ejemplo simple, es aceptable.
    std::cout << "Fecha y hora actual: " << std::put_time(std::localtime(&t_actual), "%Y-%m-%d %H:%M:%S") << std::endl;

    std::cout << "\nLa diferencia es:" << std::endl;
    std::cout << "  " << dias << " días" << std::endl;
    std::cout << "  " << horas << " horas" << std::endl;
    std::cout << "  " << minutos << " minutos" << std::endl;
    std::cout << "  " << segundos << " segundos" << std::endl;
    std::cout << "  (Total: " << total_segundos << " segundos)" << std::endl;

    // Para una aproximación en años:
    double diferencia_anios = total_segundos / (365.25 * 24 * 60 * 60); // Usando 365.25 para bisiestos
    std::cout << "  Aproximadamente " << std::fixed << std::setprecision(2) << diferencia_anios << " años" << std::endl;

    return 0;
}
/** 
int main() {
    // Mostrar el directorio actual
    cout << "Directorio actual: " << std::filesystem::current_path() << endl;

    string rutaArchivo = "./prueba.csv";

    // Obtener tamaño del archivo
    size_t tamano = obtenerTamanoArchivo(rutaArchivo);
    if (tamano == 0) return 1;
    cout << "Tamano del archivo: " << tamano << " bytes" << endl;

    // Obtener cantidad de hilos
    unsigned int num_hilos = obtenerCantidadHilos();
    cout << "Cantidad de hilos detectados: " << num_hilos << endl;

    // Calcular tamaño de bloque
    size_t tamano_bloque = calcularTamanoBloque(tamano, num_hilos);
    cout << "Tamano de cada bloque: " << tamano_bloque << " bytes" << endl;

    // 1. Vector para los hilos y para los registros de cada hilo
    vector<thread> hilos;
    // vector<vector<Registro>> registros_por_hilo(num_hilos);
    Estadisticas Resultado;
    // 2. Lanzar los hilos
    for (unsigned int i = 0; i < num_hilos; ++i) {
        size_t inicio = i * tamano_bloque;
        size_t fin = (i == num_hilos - 1) ? tamano : (inicio + tamano_bloque);
        bool esPrimerHilo = (i == 0);
        hilos.emplace_back(procesarBloque, rutaArchivo, inicio, fin, ref(Resultado), esPrimerHilo); //ref(registros_por_hilo[i])
    }

    // 3. Esperar a que todos los hilos terminen
    for (auto& h : hilos) {
        h.join();
    }

    for(int i=0;i<Resultado.Estrato.size();i++)
    Resultado.total+= Resultado.Estrato[i];
    return 0;
}*/