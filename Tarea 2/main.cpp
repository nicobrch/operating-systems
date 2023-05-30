#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>

using namespace std;

struct Objeto
{
    int peso;
    int ganancia;
};

int generarRandom(int minimo, int maximo)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(minimo, maximo);
    return distr(gen);
}

void leerArchivo(const string& nombreArchivo, vector<Objeto>& objetos)
{
    ifstream archivo(nombreArchivo);
    if (!archivo)
    {
        cerr << "No se pudo abrir el archivo" << endl;
        return;
    }
    string linea;
    while (getline(archivo, linea))
    {
        istringstream iss(linea);
        int peso, ganancia;
        Objeto obj;
        if (iss >> peso >> ganancia) {
            obj.peso = peso;
            obj.ganancia = ganancia;
            objetos.push_back(obj);
        }
    }
    archivo.close();
}


void agregarItems(vector<Objeto>& objetos)
{
    
}

int main() {
    vector<Objeto> objetos;
    string nombreArchivo = "mochila.txt";
    leerArchivo(nombreArchivo, objetos);
    
    return 0;
}
