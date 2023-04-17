#ifndef T1_SO_UTILS_H
#define T1_SO_UTILS_H
#endif //T1_SO_UTILS_H

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
#include <semaphore.h>
#include <random>
#include <vector>
#include <string>
#include <sstream>

#define FIFO1 "fifo1"
#define FIFO2 "fifo2"

using namespace std;

struct message {
    int process_id;
    char data[256];
};

int genRandom(int minimo, int maximo){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(minimo, maximo);
    return distr(gen);
}

int charArrayToInt(message msg){
    int result;
    string data;
    data.assign(msg.data);
    stringstream ss(data);
    ss >> result;
    return result;
}

void enviarMensaje(int fd, int pid, sem_t *mutex, message msg, string mensaje){
    //  Asociar atributos al struct message
    msg.process_id = pid;
    strcpy(msg.data, mensaje.c_str());
    //  Escribir mensaje a la pipe especificada
    write(fd, &msg, sizeof(message));
    //  Habilitar semaforo
    sem_post(mutex);
}

void enviarMensajeCIN(int fd, int pid, sem_t *mutex, message msg){
    //  Asociar atributos al struct message
    msg.process_id = pid;
    cin >> msg.data;
    //  Escribir mensaje a la pipe especificada
    write(fd, &msg, sizeof(message));
    //  Habilitar semaforo
    sem_post(mutex);
}

message leerMensaje(int fd, sem_t *mutex, message msg){
    //  Esperar semaforo
    sem_wait(mutex);
    //  Leer mensaje desde la PIPE2
    long n = read(fd, &msg, sizeof(message));
    if (n > 0){
        //  Imprimir mensaje
        cout << "[" << msg.process_id << "] " << msg.data << endl;
    } else {
        cerr << "Error al abrir la pipe" << endl;
    }
    return msg;
}

message leerMensajeInicial(int fd, message msg){
    //  Leer mensaje desde la PIPE2
    long n = read(fd, &msg, sizeof(message));
    if (n > 0){
        //  Imprimir mensaje
        cout << "[" << msg.process_id << "] " << msg.data << endl;
    } else {
        cerr << "Error al abrir la pipe" << endl;
    }
    return msg;
}

class Mesa {
private:
    int puntaje;
public:
    Mesa(){
        this->puntaje = genRandom(3, 21);
    }
    int getPuntaje() const { return this->puntaje; }
};

class Jugador {
private:
    int id;
    int monto;
    vector<int> cartas;
public:
    Jugador(int id, int mnt){
        this->id = id;
        this->monto = mnt;
        this->cartas.push_back(genRandom(1, 10));
        this->cartas.push_back(genRandom(1, 10));
    }
    int getId() const { return this->id; }
    int getMonto() const { return this->monto; }
    vector<int> getCartas() { return this->cartas; }
    unsigned long getCantidadCartas() const { return this->cartas.size(); }
    int getSumaCartas() {
        int suma = 0;
        for (int i=0; i<cartas.size(); i++){
            suma += cartas[i];
        }
        return suma;
    }
    void setMonto(int mnt){
        this->monto = mnt;
    }
    void agregarCarta(){
        this->cartas.push_back(genRandom(1, 10));
    }
    string imprimirCartas(){
        string auxMsj = "Sus cartas son: ";
        for (int i=0; i<getCantidadCartas(); i++){
            auxMsj += to_string(this->cartas[i]);
            if (i != getCantidadCartas()-1){
                auxMsj += " - ";
            }
        }
        auxMsj += "\nDigite 1 si desea obtener una carta más, 0 si desea apostar con las que ya posee.";
        return auxMsj;
    }
};
