#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>

#include "leer_csv.h"

using namespace std;

/*Funciones para leer archivo CSV*/
void leer_archivo() {
    string rutaArchivo = "../datos_prueba.csv";
    // Obtener tamaño del archivo
    size_t tamano = obtenerTamanoArchivo(rutaArchivo);
    if (tamano == 0) return;
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
   return;
}

/*Funciones para mostrar el menú y procesar opciones*/
void linea_separadora(){
    cout<<"--------------------------------------------------\n";
}
void mostrar_menu(){
    int numero;
    regresar:
    linea_separadora();
    cout<<"Menu de opciones\n";
    cout<<"1. ¿Cuantas personas pertenecen a cada estrato social?\n";
    cout<<"2. ¿Qué porcentaje de la población pertenece a cada estrato social?\n";
    cout<<"3. ¿Cuál es la edad promedio según cada especie y género?\n";
    cout<<"4. ¿Cuál es la edad mediana según cada especie y género? \n";
    cout<<"5. ¿Qué proporción de la población tiene menos de 18 años, entre 18–35, 36–60, más de 60 según especie y género?\n";
    cout<<"6. ¿Cuál es la pirámide de edades de la población según especie, género?\n";
    cout<<"7. ¿Cuál es el índice de dependencia (menores de 15 + mayores de 64) / población en edad de trabajar?\n";
    cout<<"8. Acorde a la información de origen y destino, determine los 10000 poblados con más viajes, con esta información el rey construirá o repará los caminos que unirían estas ciudades/aldeas. \n";
    cout<<"9. Salir\n\n";
    cout<<"Ingrese el numero de la opcion que desea consultar: ";
    
    cin>>numero;
    linea_separadora();
    switch (numero)
    {
    case 1:
        cout<<"Presionaste el numero uno\n";
        goto regresar;
        break;
    case 2:
        cout<<"Presionaste el numero dos\n";
        goto regresar;
        break;
    case 3:
        cout<<"Presionaste el numero tres\n";
        goto regresar;
        break;
    case 4:
        cout<<"Presionaste el numero cuatro\n";
        goto regresar;
        break;
    case 5:
        cout<<"Presionaste el numero cinco\n";
        goto regresar;
        break;
    case 6:
        cout<<"Presionaste el numero seis\n";
        goto regresar;
        break;
    case 7:
        cout<<"Presionaste el numero siete\n";
        goto regresar;
        break;
    case 8:
        cout<<"Presionaste el numero ocho\n";
        goto regresar;
        break;
    case 9:
        cout<<"Humano te voy a extrañar\n";
        system("pause");
        return;
        break;    
    default:
        cout<<"Humano estupido te dije que entre 1 y 8 o 9 para salir\n";
        goto regresar;
        break;
    }
}

int main() {
    //leer_archivo();

    mostrar_menu();
    system("pause");
    return 0;
}