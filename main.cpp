#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm>

using namespace std;

// Estructura para almacenar las estadísticas
struct Estadisticas
{
    vector<int> Estrato={0,0,0,0,0,0,0,0,0,0};
    float total=0;
    map<pair<string,string>,vector<int>> Edades;
};

vector<int> Hoy;
mutex Bloqueo;

/*Funciones para mostrar el menú y procesar opciones*/
void linea_separadora(){
    cout<<"--------------------------------------------------\n";
}
void mostrar_menu(Estadisticas Resultado){
    cout<<fixed<<setprecision(2);
    
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
    int numero;
    cin>>numero;
    linea_separadora();
    float dependencia = 0;
    float trabajadores = 0;
    vector<int> edad={0,0,0,0,0,0,0,0,0,0};
    switch (numero)
    {
    case 1:
        cout<<"Presionaste el numero uno\n";
        //respuesta 1 y 2
        for(int i=0; i<10;i++)
            cout<<Resultado.Estrato[i]<<" personas pertenecen al estrato social "<<i<<endl;
        goto regresar;
        break;
    case 2:
        cout<<"Presionaste el numero dos\n";
        for(int i=0; i<10;i++)
            cout<<(Resultado.Estrato[i]/Resultado.total)*100<<"% de la población pertenecen al estrato social "<<i<<endl;
        goto regresar;
        break;
    case 3:
        cout<<"Presionaste el numero tres\n";
        //respuesta 3,4,5

        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            float suma=0;
            float menor18=0;
            float menor38=0;
            float menor60=0;
            float mayor60=0;
            //cout<<"Para la especie "<<especie<<", "<<genero<<" la edad media y la mediana es:";
            int max=edades.size();
            for (int i=0; i < max;i++){
                suma+=edades[i];
                if(edades[i]<18)
                    menor18+=1;
                else if(edades[i]<38)
                    menor38+=1;
                else if(edades[i]<60)
                    menor60+=1;
                else
                    mayor60+=1;
            }
            sort(edades.begin(), edades.end());
            float respuesta3=suma/edades.size();

            cout<<"La edad promedio de la especie "<<especie<<", "<<genero<<" es: "<<respuesta3<<endl;
        }
        goto regresar;
        break;
    case 4:
        cout<<"Presionaste el numero cuatro\n";
        //respuesta 3,4,5

        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            //cout<<"Para la especie "<<especie<<", "<<genero<<" la edad media y la mediana es:";
            sort(edades.begin(), edades.end());
            float respuesta4=edades[edades.size()/2];
            //cout<<"Adicionalmente la propoción de menor de 18, entre 18-38, entre 38 y 60, y mayor de 60 anios es respectivamente:"<<endl;

            cout<<"La edad mediana de la especie "<<especie<<", "<<genero<<" es: "<<respuesta4<<endl;
        }
        goto regresar;
        break;
    case 5:
        cout<<"Presionaste el numero cinco\n";
        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            float menor18=0;
            float menor38=0;
            float menor60=0;
            float mayor60=0;
            cout<<"Para la especie "<<especie<<", "<<genero<<endl;
            int max=edades.size();
            for (int i=0; i < max;i++){
                if(edades[i]<18)
                    menor18+=1;
                else if(edades[i]<38)
                    menor38+=1;
                else if(edades[i]<60)
                    menor60+=1;
                else
                    mayor60+=1;
            }
            cout<<"Menor de 18: "<<(menor18/edades.size())*100<<"%"<<endl;
            cout<<"Entre 18 y 38: "<<(menor38/edades.size())*100<<"%"<<endl;
            cout<<"Entre 38 y 60: "<<(menor60/edades.size())*100<<"%"<<endl;
            cout<<"Mayor de 60: "<<(mayor60/edades.size())*100<<"%"<<endl;

            cout<<endl<<endl;
        }
        goto regresar;
        break;
    case 6:
        
        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            int max=edades.size();
            for (int i=0; i < max;i++){
                if (edades[i]<11)
                    edad[0]++;
                else if (edades[i]<21)
                    edad[1]++;
                else if (edades[i]<31)
                    edad[2]++;
                else if (edades[i]<41)
                    edad[3]++;
                else if (edades[i]<51)
                    edad[4]++;
                else if (edades[i]<61)
                    edad[5]++;
                else if (edades[i]<71)
                    edad[6]++;
                else if (edades[i]<81)
                    edad[7]++;
                else if (edades[i]<91)
                    edad[8]++;
                else if (edades[i]>90)
                    edad[9]++;

            }
            for (int i =0;i <edad.size();i++)
                edad[i]=edad[i]/edad.size();
            cout<<"Para la especie "<<especie<<", "<<genero<<" el rango edad es:"<<endl;
            for(int i =0;i<edad.size();i++)
            {
                if(i!=9)
                cout<<" ";
                cout<<i<<"-"<<(i+1)*10<<":";
                for(int j=0;j<edad[i];j++)
                    cout<<"#";
                cout<<endl;
            }
            cout<<endl<<endl;
        }
        goto regresar;
        break;
    case 7:
        cout<<"Presionaste el numero siete\n";
        //respuesta 7
        

        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            int max=edades.size();
            for (int i=0; i < max;i++){
                if(edades[i]<15 || edades[i]>64)
                    dependencia+=1;
                else
                    trabajadores+=1;
            }
        }
        cout<<"La proporción de personas con dependencia es del: "<<dependencia/trabajadores<<endl;
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

// Función para obtener la fecha y hora actual desglosada
vector <int> obtenerFechaHoraActualDesglosada() {
    auto now_time_point = std::chrono::system_clock::now();

    std::time_t tiempo_actual_t = std::chrono::system_clock::to_time_t(now_time_point);

    std::tm* tm_info = std::localtime(&tiempo_actual_t);

    if (tm_info == nullptr) {
        throw std::runtime_error("Error al obtener la fecha y hora local. (std::localtime fallo)");
    }

    int anio = tm_info->tm_year + 1900;
    int mes = tm_info->tm_mon + 1;     
    int dia = tm_info->tm_mday;        
    int hora = tm_info->tm_hour;       
    int minuto = tm_info->tm_min;      
    int segundo = tm_info->tm_sec;     

    return {anio,mes,dia,hora,minuto,segundo};
}

// Parsea una cadena de fecha y hora en formato ISO 8601 (YYYY-MM-DDTHH:MM:SS) a un vector de enteros
// que contiene el año, mes, día, hora, minuto y segundo.
vector<int> obtenerfecha(string& fecha_str) {
    tm tm_info = {}; 
    stringstream ss(fecha_str);

    ss >> std::get_time(&tm_info, "%Y-%m-%dT%H:%M:%S");

    if (ss.fail()) {
        throw std::runtime_error("Error al parsear la cadena de fecha y hora: Formato incorrecto o fecha invalida.");
    }

    
    int anio = tm_info.tm_year; 
    if (anio < 1900) { 
        anio += 1900;
    }
    
    int mes = tm_info.tm_mon + 1; 
    int dia = tm_info.tm_mday;
    int hora = tm_info.tm_hour;
    int minuto = tm_info.tm_min;
    int segundo = tm_info.tm_sec;

    return {anio,mes,dia,hora,minuto,segundo};
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

int edad(vector<int> hoy, vector<int> nacimiento){
    bool cumple=true;
    if (hoy[1]>nacimiento[1])
        cumple = false;
    else if (hoy[1]==nacimiento[1] && hoy[2]>nacimiento[2])
        cumple = false;
    else if (hoy[2]==nacimiento[2] && hoy[3]>nacimiento[3])
        cumple = false;
    else if (hoy[3]==nacimiento[3] && hoy[4]>nacimiento[4])
        cumple = false;
    else if (hoy[5]>nacimiento[5])
        cumple = false;
    
    return (cumple)? hoy[0]-nacimiento[0]:hoy[0] -nacimiento[0]-1;

}

string aMayusculas(string s) {
    transform(s.begin(), s.end(), s.begin(),[](unsigned char c){ return toupper(c); });
    return s;
}

void parsearLineaCSV(const string& linea, Estadisticas& Resultado) {

    size_t inicio = 0, fin = 0;
    string campos[8];
    int campoActual = 0;

    while (campoActual < 8 && inicio < linea.size()) {
        // Buscar la primera comilla
        inicio = linea.find('"', fin);
        if (inicio == string::npos) break;
        // Buscar la segunda comilla
        fin = linea.find('"', inicio + 1);
        if (fin == string::npos) break;
        // Extraer el campo entre comillas
        campos[campoActual++] = linea.substr(inicio + 1, fin - inicio - 1);
        // Avanzar al siguiente separador
        fin = linea.find(';', fin);
        if (fin == string::npos) fin = linea.size();
    }
    campos[1]=aMayusculas(campos[1]);
    campos[2]=aMayusculas(campos[2]);

    lock_guard<mutex> lock(Bloqueo);
    Resultado.Estrato[int(campos[6][0])-48]+=1;
    Resultado.Edades[{campos[1],campos[2]}].push_back(edad(Hoy,obtenerfecha(campos[5])));


}

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
 
void leer_csv(string rutaArchivo){
    Hoy=obtenerFechaHoraActualDesglosada();
    // Mostrar el directorio actual
    //cout << "Directorio actual: " << std::filesystem::current_path() << endl;

    // Obtener tamaño del archivo
    size_t tamano = obtenerTamanoArchivo(rutaArchivo);
    if (tamano == 0) return;
    //cout << "Tamano del archivo: " << tamano << " bytes" << endl;

    // Obtener cantidad de hilos
    unsigned int num_hilos = obtenerCantidadHilos();
    //cout << "Cantidad de hilos detectados: " << num_hilos << endl;

    // Calcular tamaño de bloque
    size_t tamano_bloque = calcularTamanoBloque(tamano, num_hilos);
    //cout << "Tamano de cada bloque: " << tamano_bloque << " bytes" << endl;

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

    for(int i=0;i<10;i++)
        Resultado.total+= Resultado.Estrato[i];

    mostrar_menu(Resultado);
}


int main() {
    string rutaArchivo = "E:/Proyectos/ProyectoParalelo/datos_prueba.csv";
    leer_csv(rutaArchivo);
    return 0;
}