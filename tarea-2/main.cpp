#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <vector>

using namespace std;

int pesoMaximo;
int numeroObjetos;

struct Objeto
{
    int ganancia;
    int peso;
};

struct Mochila
{
    int pesoMaximo = 5000;
    int pesoActual = 0;
    int mejorGanancia = 0;
    vector<Objeto> objetos;
};

mutex mtx;
sem_t semaforo;

int generarRandom(int minimo, int maximo)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(minimo, maximo);
    return distr(gen);
}

void leerArchivo(const string& nombreArchivo, vector<Objeto>& objetos, int& mejorGanancia)
{
    // Rubrica ID 1
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

    // Rubrica ID 3
    while (clock() - inicio < ciclosEsperados)
    {
        // Rubrica ID 2
        numeroObjetos = objetos.size() - 1;
        int rand = generarRandom(1, numeroObjetos);
        Objeto objetoRandom = objetos.at(rand);
        objetos.erase(it + rand);
        // Rubrica ID 3
        if (objetoRandom.ganancia > mochila.mejorGanancia)
        {
            cout << "La mejor ganancia es: " << objetoRandom.ganancia << endl;
            mochila.mejorGanancia = objetoRandom.ganancia;
        }
        // Rubrica ID 2
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

// Rubrica ID 5
void agregarObjetosSincronizado(vector<Objeto>& objetos, Mochila& mochila, int K)
{
    clock_t inicio = clock();
    clock_t ciclosEsperados = K * CLOCKS_PER_SEC;
    auto it = objetos.begin();

    // Rubrica ID 6
    while (clock() - inicio < ciclosEsperados)
    {
        numeroObjetos = objetos.size() - 1;
        int rand = generarRandom(1, numeroObjetos);

        sem_wait(&semaforo);

        Objeto objetoRandom = objetos.at(rand);
        objetos.erase(it + rand);

        sem_post(&semaforo);

        // Rubrica ID 6
        if (objetoRandom.ganancia > mochila.mejorGanancia)
        {
            lock_guard<mutex> lock(mtx);
            cout << "La mejor ganancia es: " << objetoRandom.ganancia << endl;
            mochila.mejorGanancia = objetoRandom.ganancia;
        }
        if (mochila.pesoActual + objetoRandom.peso > mochila.pesoMaximo)
        {
            lock_guard<mutex> lock(mtx);
            cout << "Se paso del peso maximo, su peso actual es: " <<
            mochila.pesoActual << "\ny si se le suma el objeto de peso: " <<
            objetoRandom.peso << "\nse pasa del maximo que es: " << mochila.pesoMaximo << endl;
            break;
        }
        lock_guard<mutex> lock(mtx);
        mochila.objetos.push_back(objetoRandom);
        mochila.pesoActual += objetoRandom.peso;
    }

    cout << "+ Su mejor ganancia es: " << mochila.mejorGanancia << " y su peso: " << mochila.pesoActual << endl;
    cout << "------" << endl;
}

int main() {
    vector<Objeto> objetos;
    int mejorGanancia = 0, N, K;
    string nombreArchivo = "mochila.txt";
    leerArchivo(nombreArchivo, objetos, mejorGanancia);
    pesoMaximo = objetos.front().peso;
    numeroObjetos = objetos.front().ganancia - 1;

    cout << "--- Agregar Objetos ---" << endl;
    Mochila mochila;
    agregarObjetos(objetos, mochila, 1);

    // Rubrica ID 6
    cout << "--- Agregar Objetos con Threads ---" << endl;
    cout << "> Ingrese un numero N de threads" << endl;
    cin >> N;
    cout << "> Ingrese una cantidad K entera de segundos" << endl;
    cin >> K;
    vector<Mochila> mochilas(N);
    sem_init(&semaforo, 0, 1);
    vector<thread> threads(N);

    // Rubrica ID 4
    for (int i=0; i<N; i++){
        threads[i] = thread(agregarObjetosSincronizado, ref(objetos), ref(mochilas[i]), K);
    }
    for (auto &th : threads){   
        th.join();
    }

    sem_destroy(&semaforo);

    // Rubrica ID 7
    cout << "--- Agregar Objetos con 4 a la vez ---" << endl;
    cout << "> Ingrese un numero N de threads" << endl;
    cin >> N;
    cout << "> Ingrese una cantidad K entera de segundos" << endl;
    cin >> K;
    vector<Mochila> mochilasCuatro(N);
    sem_init(&semaforo, 0, 1);
    vector<thread> threadsCuatro(N);

    for (int i=0; i<N; i++){
        threads[i] = thread(agregarObjetosSincronizado, ref(objetos), ref(mochilas[i]), 3);
    }
    for (auto &th : threads){
        th.join();
    }

    sem_destroy(&semaforo);

    return 0;
}
