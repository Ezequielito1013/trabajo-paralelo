#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>

#include "leer_csv.h"

using namespace std;

int main() {
    string rutaArchivo = "../datos_prueba.csv";

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
    vector<vector<Registro>> registros_por_hilo(num_hilos);

    // 2. Lanzar los hilos
    for (unsigned int i = 0; i < num_hilos; ++i) {
        size_t inicio = i * tamano_bloque;
        size_t fin = (i == num_hilos - 1) ? tamano : (inicio + tamano_bloque);
        bool esPrimerHilo = (i == 0);
        hilos.emplace_back(procesarBloque, rutaArchivo, inicio, fin, ref(registros_por_hilo[i]), esPrimerHilo);
    }

    // 3. Esperar a que todos los hilos terminen
    for (auto& h : hilos) {
        h.join();
    }

    // 4. (Opcional) Unir todos los registros en un solo vector
    size_t total_registros = 0;
    Registro primer_registro, ultimo_registro;
    bool primer_encontrado = false;

    for (const auto& vec : registros_por_hilo) {
        if (!vec.empty()) {
            if (!primer_encontrado) {
                primer_registro = vec.front();
                primer_encontrado = true;
            }
            ultimo_registro = vec.back();
            total_registros += vec.size();
        }
    }

    cout << "Total de registros leídos: " << total_registros << endl;

    if (primer_encontrado) {
        cout << "\nPrimer registro:" << endl;
        cout << "ID: " << primer_registro.identificador
             << ", Especie: " << primer_registro.especie
             << ", Nombre: " << primer_registro.nombre << endl;

        cout << "\nÚltimo registro:" << endl;
        cout << "ID: " << ultimo_registro.identificador
             << ", Especie: " << ultimo_registro.especie
             << ", Nombre: " << ultimo_registro.nombre << endl;
    }

    /* Imprimir la lista de identificadores leídos
    cout << "\nLista de identificadores leídos:" << endl;
    for (size_t i = 0; i < todos_los_registros.size(); ++i) {
        cout << i + 1 << ": " << todos_los_registros[i].identificador << endl;
    }
    */
    system("pause");
    return 0;
}