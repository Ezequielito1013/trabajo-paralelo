#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>
#include "leer_csv.h"
using namespace std;

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
Registro parsearLineaCSV(const string& linea) {
    Registro reg;
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

    // Asignar los campos extraídos al struct Registro
    reg.identificador    = campos[0];
    reg.especie          = campos[1];
    reg.genero           = campos[2];
    reg.nombre           = campos[3];
    reg.apellido         = campos[4];
    reg.fechaNacimiento  = campos[5];
    reg.cpOrigen         = campos[6];
    reg.cpDestino        = campos[7];

    return reg;
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
void procesarBloque(const string& rutaArchivo, size_t inicio, size_t fin, vector<Registro>& registros, bool esPrimerHilo) {
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
        registros.push_back(parsearLineaCSV(linea));
        posActual = archivo.tellg();
        if (archivo.eof()) break;
    }
}
