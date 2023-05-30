#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <mutex>
#include <semaphore.h>
#include <thread>

using namespace std;

struct Objeto
{
    int ganancia;
    int peso;
};

struct Mochila
{
    int pesoMaximo = 0;
    int pesoActual = 0;
    int mejorGanancia = 0;
    vector<Objeto> objetos;
};

mutex mtx;
sem_t sem;
int numeroObjetos = 200;
int pesoMaximo = 5000;

int generarRandom(int minimo, int maximo)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(minimo, maximo);
    return distr(gen);
}

void leerArchivo(const string& nombreArchivo, vector<Objeto>& objetos, int& mejorGanancia)
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
        int ganancia, peso;
        Objeto obj = {};
        if (iss >> ganancia >> peso)
        {
            obj.ganancia = ganancia;
            obj.peso = peso;
            if (ganancia > mejorGanancia) mejorGanancia = ganancia;
            objetos.push_back(obj);
        }
    }
    archivo.close();
}

void agregarObjetos(vector<Objeto>& objetos, Mochila& mochila, int K)
{
    clock_t inicio = clock();
    clock_t ciclosEsperados = K * CLOCKS_PER_SEC;
    auto it = objetos.begin();

    while (clock() - inicio < ciclosEsperados)
    {
        int rand = generarRandom(1, numeroObjetos);
        Objeto objetoRandom = objetos.at(rand);
        objetos.erase(it + rand);
        if (objetoRandom.ganancia > mochila.mejorGanancia)
        {
            cout << "La mejor ganancia es: " << objetoRandom.ganancia << endl;
            mochila.mejorGanancia = objetoRandom.ganancia;
        }
        if (mochila.pesoActual + objetoRandom.peso > mochila.pesoMaximo)
        {
            cout << "Se paso del peso maximo, su peso actual es: " <<
            mochila.pesoActual << "\ny si se le suma el objeto de peso: " <<
            objetoRandom.peso << "\nse pasa del maximo que es: " << mochila.pesoMaximo << endl;
            break;
        }
        mochila.objetos.push_back(objetoRandom);
        mochila.pesoActual += objetoRandom.peso;
    }

    cout << "+ Su mejor ganancia es: " << mochila.mejorGanancia << " y su peso: " << mochila.pesoActual << endl;
}

void agregarObjetosSincronizado(vector<Objeto>& objetos, Mochila& mochila, int K)
{
    clock_t inicio = clock();
    clock_t ciclosEsperados = K * CLOCKS_PER_SEC;
    auto it = objetos.begin();

    while (clock() - inicio < ciclosEsperados) {
        int rand = generarRandom(1, numeroObjetos);
        sem_wait(&sem); // Esperar a que el semáforo esté disponible
        mtx.lock(); // Bloquear el acceso al vector de objetos
        Objeto objetoRandom = objetos.at(rand);
        objetos.erase(it + rand);
        mtx.unlock(); // Desbloquear el acceso al vector de objetos
        sem_post(&sem); // Liberar el semáforo

        if (objetoRandom.ganancia > mochila.mejorGanancia) {
            cout << "La mejor ganancia es: " << objetoRandom.ganancia << endl;
            mochila.mejorGanancia = objetoRandom.ganancia;
        }
        if (mochila.pesoActual + objetoRandom.peso > mochila.pesoMaximo) {
            cout << "Se paso del peso maximo" << endl;
            break;
        }

        mochila.objetos.push_back(objetoRandom);
        mochila.pesoActual += objetoRandom.peso;
    }
    cout << "+ Su mejor ganancia es: " << mochila.mejorGanancia << " y su peso: " << mochila.pesoActual << endl;
}

int main() {
    vector<Objeto> objetos;
    int mejorGanancia = 0;
    string nombreArchivo = "mochila.txt";
    leerArchivo(nombreArchivo, objetos, mejorGanancia);

    cout << "--- Agregar Objetos ---" << endl;
    Mochila mochila;
    agregarObjetos(objetos, mochila, 1);

    const int NThreads = 5;
    vector<thread> threads(NThreads);
    vector<Mochila> mochilasThreads;
    sem_init(&sem, 0, 1);
    cout << "--- Agregar Objetos Sincronizados ---" << endl;
    for (int i = 0; i < NThreads; ++i) {
        cout << "<< Thread " << i << " >>" << endl;
        threads.at(i) = thread([&objetos, &mochilasThreads, i]() {
            agregarObjetosSincronizado(objetos, mochilasThreads[i], 3);
        });
    }
    for (int i = 0; i < NThreads; ++i) {
        threads[i].join(); // Espera a que todos los threads terminen
    }
    sem_destroy(&sem);
    for (int i = 0; i < NThreads; ++i) {
        cout << "Mochila " << i + 1 << " - Mejor ganancia: " << mochilasThreads.at(i).mejorGanancia << std::endl;
    }

    return 0;
}
