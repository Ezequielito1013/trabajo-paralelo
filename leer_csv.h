#include <string>
#include <vector>
using namespace std;

// Estructura para almacenar cada registro del CSV
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

size_t obtenerTamanoArchivo(const string& rutaArchivo);
unsigned int obtenerCantidadHilos();
size_t calcularTamanoBloque(size_t tamanoArchivo, unsigned int numHilos);
Registro parsearLineaCSV(const string& linea);
void procesarBloque(const string& rutaArchivo, size_t inicio, size_t fin, vector<Registro>& registros, bool esPrimerHilo);