#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <omp.h>   

using namespace std;

struct Estadisticas
{
    vector<int> Estrato={0,0,0,0,0,0,0,0,0,0};
    float total=0;
    map<pair<string,string>,vector<int>> Edades;
    map<string,int> viajes;
    void merge(const Estadisticas& other) {
        for (int i = 0; i < 10; ++i) {
            Estrato[i] += other.Estrato[i];
        }

        for (const auto& entry : other.Edades) {
            Edades[entry.first].insert(Edades[entry.first].end(),
                                         entry.second.begin(), entry.second.end());
        }
        for (const auto& entry : other.viajes) {
            viajes[entry.first] += entry.second;
        }
    }
};

vector<int> Hoy;

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
    vector<float> edad={0,0,0,0,0,0,0,0,0,0};
    vector<pair<string,int>> pares;
    int j = 0;
    switch (numero)
    {
    case 1:
        cout<<"Presionaste el numero uno\n";
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

        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            float suma=0;
            int max=edades.size();
            for (int i=0; i < max;i++){

                suma+=edades[i];
            }
            cout<<endl;
            float respuesta3=suma/edades.size();

            cout<<"La edad promedio de la especie "<<especie<<", "<<genero<<" es: "<<respuesta3<<endl;
        }
        goto regresar;
        break;
    case 4:
        cout<<"Presionaste el numero cuatro\n";

        for (const auto& combinacion : Resultado.Edades)
        {
            string especie=combinacion.first.first;
            string genero=combinacion.first.second;
            vector<int> edades = combinacion.second;
            sort(edades.begin(), edades.end());
            int respuesta4=edades[(edades.size())/2];

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
            max=edad.size();
            for (int i =0;i < max ;i++)
                edad[i]=edad[i]/edades.size();
            cout<<"Para la especie "<<especie<<", "<<genero<<" el rango edad es:"<<endl;
            for(int i =0;i<max;i++)
            {
                if(i!=9)
                cout<<" ";
                cout<<i<<"-"<<(i+1)*10<<":";
                cout<<"     ";
                for(int j=1;j<=(max*2);j++)
                    if(j>10-edad[i]*10 && j<10+edad[i]*10)
                        cout<<"#";
                    else
                        cout<<" ";
                cout<<endl;
            }
            cout<<endl<<endl;
        }
        goto regresar;
        break;
    case 7:
        cout<<"Presionaste el numero siete\n";
        

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
        for (const auto& par : Resultado.viajes){
            pares.push_back(par);
        }
        sort(pares.begin(), pares.end(),[](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            return a.first < b.first;
            });
        cout<<"Los 10.000 ciudades que tienen mas viajes son:"<<endl;
        j = pares.size();
        for (int i=0;i<10000 && i<j;i++)
        {
            cout<<"Hacia "<<pares[i].first<<" son "<<pares[i].second<<" viajes."<<endl;
        }
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

vector <int> obtenerFechaHoraActualDesglosada() {
    auto now_time_point = chrono::system_clock::now();
    time_t tiempo_actual_t = chrono::system_clock::to_time_t(now_time_point);
    tm* tm_info = localtime(&tiempo_actual_t);
    if (tm_info == nullptr) {
        throw runtime_error("Error al obtener la fecha y hora local. (localtime fallo)");
    }
    int anio = tm_info->tm_year + 1900;
    int mes = tm_info->tm_mon + 1;
    int dia = tm_info->tm_mday;
    return {anio,mes,dia};
}

vector<int> obtenerfecha(const string& fecha_str) {
    tm tm_info = {};
    stringstream ss(fecha_str);
    ss >> get_time(&tm_info, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        throw runtime_error("Error al parsear la cadena de fecha y hora: Formato incorrecto o fecha invalida.");
    }
    int anio = tm_info.tm_year + 1900;
    int mes = tm_info.tm_mon + 1;
    int dia = tm_info.tm_mday;
    return {anio,mes,dia};
}

size_t obtenerTamanoArchivo(const string& rutaArchivo) {
    ifstream archivo(rutaArchivo, ios::binary | ios::ate);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return 0;
    }
    return static_cast<size_t>(archivo.tellg());
}

int edad(vector<int> hoy, vector<int> nacimiento){
    bool cumple = true;
    if (hoy[1]<nacimiento[1])
        cumple = false;
    else if (hoy[2]<nacimiento[2])
        cumple = false;
    
    return (cumple || hoy[0]==nacimiento[0]) ? hoy[0] - nacimiento[0] : hoy[0] - nacimiento[0] - 1;
}

string aMayusculas(string s) {
    transform(s.begin(), s.end(), s.begin(),[](unsigned char c){ return toupper(c); });
    return s;
}

void parsearLineaCSV(const string& linea, Estadisticas& resultado_local) {
    size_t inicio = 0, fin = 0;
    string campos[8];
    int campoActual = 0;

    while (campoActual < 8 && inicio < linea.size()) {
        inicio = linea.find('"', fin);
        if (inicio == string::npos) break; 
        fin = linea.find('"', inicio + 1);
        if (fin == string::npos) break; 
        campos[campoActual++] = linea.substr(inicio + 1, fin - inicio - 1);
        fin = linea.find(';', fin);
        if (fin == string::npos) fin = linea.size();
    }

    if (campoActual < 8) {
        return;
    }
    string especie = aMayusculas(campos[1]);
    string genero = aMayusculas(campos[2]);
    string ciudad_destino = campos[7];

    if (!campos[6].empty() && isdigit(campos[6][0])) {
        resultado_local.Estrato[int(campos[6][0])-48]++;
    }

    try {
        resultado_local.Edades[{especie, genero}].push_back(edad(Hoy,obtenerfecha(campos[5])));
    } catch (const runtime_error& e) {
    }

    resultado_local.viajes[ciudad_destino]++; 
    resultado_local.total++; 
}


void procesarBloqueOMP(const string& rutaArchivo, long long inicio_bloque, long long fin_bloque, Estadisticas& resultado_local, bool esPrimerHilo) {
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo en el hilo OMP." << endl;
        return;
    }

    archivo.seekg(inicio_bloque);
    
    string linea;
    long long posActual; 
    if (inicio_bloque == 0 && esPrimerHilo) {
        string dummy;
        getline(archivo, dummy);
    }
    while (getline(archivo, linea)) {
        posActual = archivo.tellg();

        if (posActual >= fin_bloque && fin_bloque != static_cast<long long>(obtenerTamanoArchivo(rutaArchivo))) {
            if (linea.empty()==false)
                parsearLineaCSV(linea, resultado_local);
            break;
        }

        if (linea.empty()){ 
            continue;
        }
        parsearLineaCSV(linea, resultado_local);
    }
    archivo.close();
}


void leer_csv(string rutaArchivo){
    Hoy = obtenerFechaHoraActualDesglosada();
    size_t tamano = obtenerTamanoArchivo(rutaArchivo);
    if (tamano == 0) {
        cerr << "El archivo está vacío o no se pudo abrir." << endl;
        return;
    }

    int num_hilos = omp_get_max_threads(); 

    cout << "Cantidad de hilos a usar: " << num_hilos << endl;

    long long tamano_bloque = tamano / num_hilos;


    Estadisticas ResultadoFinal; 

    #pragma omp parallel
    {
        Estadisticas resultado_local; 

        int thread_id = omp_get_thread_num();
        long long inicio_bloque_hilo = thread_id * tamano_bloque;
        long long fin_bloque_hilo = (thread_id == num_hilos - 1) ? tamano : (inicio_bloque_hilo + tamano_bloque);

        if (thread_id != 0) {
            ifstream temp_file_finder(rutaArchivo, ios::binary);
            temp_file_finder.seekg(inicio_bloque_hilo);
            string dummy;
            getline(temp_file_finder, dummy);
            inicio_bloque_hilo = temp_file_finder.tellg();
            temp_file_finder.close();
        }
        procesarBloqueOMP(rutaArchivo, inicio_bloque_hilo, fin_bloque_hilo, resultado_local, (thread_id == 0));

        #pragma omp critical
        {
            ResultadoFinal.merge(resultado_local);
        }
    } 

    for(int count : ResultadoFinal.Estrato) {
        ResultadoFinal.total += count;
    }
    
    mostrar_menu(ResultadoFinal);
}


int main() {
    string rutaArchivo = "./eldoria.csv";
    auto start_time = chrono::high_resolution_clock::now();
    leer_csv(rutaArchivo);

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "\nTiempo total de ejecución: " << duration.count() / 1000.0 << " segundos." << endl;

    return 0;
}