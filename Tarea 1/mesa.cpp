#include "utils.h"

int main() {
    // Variables
    int fd1, fd2, pid = getpid();
    long n;
    vector<int> cartas;
    sem_t *mutex;
    message msg{};
    string auxMsj;
    bool gameOver = false;
    auto mesa = new Mesa;

    // Crear pipes
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    // Abrir pipes
    fd1 = open(FIFO1, O_RDWR);
    fd2 = open(FIFO2, O_RDWR);

    // Crear semaforo
    mutex = sem_open("mutex", O_CREAT, 0666, 1);

    cout << "Bienvenido al juego del 21.\nEsperando que se conecten los jugadores..." << endl;

    //  Leer mensaje desde la PIPE1
    msg = leerMensajeInicial(fd1, msg);

    //  Crear jugador que se ha conectado
    auto jugador1 = new Jugador(msg.process_id, 1000);

    //  Enviar mensaje a la PIPE2
    enviarMensaje(fd2, pid, mutex, msg, "Hola Jugador " + to_string(jugador1->getId()) + "! Bienvenido al" +
        " juego del 21.\nUsted cuenta con un monto total de " + to_string(jugador1->getMonto()) + ". Por favor " +
        "indique cuánto desea apostar.");

    //  Leer mensaje desde la PIPE1
    msg = leerMensaje(fd1, mutex, msg);

    //  Modificar monto
    int apuesta = charArrayToInt(msg);
    auxMsj = jugador1->imprimirCartas();

    //  Enviar cartas sus cartas al jugador por la PIPE2
    enviarMensaje(fd2, pid, mutex, msg, auxMsj);

    //  Leer mensaje desde la PIPE1
    msg = leerMensaje(fd1, mutex, msg);

    int opcion = charArrayToInt(msg);
    if (opcion == 1)
    {
        //  Desea una carta adicional.
        jugador1->agregarCarta();
        auxMsj = jugador1->imprimirCartas();
        //  Enviar cartas sus cartas al jugador por la PIPE2
        enviarMensaje(fd2, pid, mutex, msg, auxMsj);
    }
    else
    {
        //  Desea apostar con las que ya tiene.
        int puntajeMesa = mesa->getPuntaje(), puntajeJugador = jugador1->getSumaCartas();

        if (puntajeJugador > puntajeMesa && puntajeJugador <= 21)
        {
            //  Ganó el juego
            //  Sumar lo apostado a su monto total
            jugador1->setMonto(jugador1->getMonto() + (apuesta * 2));
            //  Mensaje ganador
            auxMsj = "El puntaje de la mesa es de: " + to_string(puntajeMesa) +
                     "\nEl puntaje del jugador es de: " + to_string(puntajeJugador) +
                     "\nJugador " + to_string(jugador1->getId()) + " ha ganado!" +
                     "\nSu nuevo monto es de: " + to_string(jugador1->getMonto()) + "\n";
            //  Enviar mensaje de victoria por la PIPE2
            enviarMensaje(fd2, pid, mutex, msg, auxMsj);
        }
        else if (puntajeJugador > 21)
        {
            //  Sumar lo apostado a su monto total
            jugador1->setMonto(jugador1->getMonto() - apuesta);
            //  Mensaje ganador
            auxMsj = "El puntaje de la mesa es de: " + to_string(puntajeMesa) +
                     "\nEl puntaje del jugador es de: " + to_string(puntajeJugador) +
                     "\nJugador " + to_string(jugador1->getId()) + " ha perdido por pasarse del 21 :(" +
                     "\nSu nuevo monto es de: " + to_string(jugador1->getMonto()) + "\n";
            //  Enviar mensaje de victoria por la PIPE2
            enviarMensaje(fd2, pid, mutex, msg, auxMsj);
        }
        else
        {
            //  Sumar lo apostado a su monto total
            jugador1->setMonto(jugador1->getMonto() - apuesta);
            //  Mensaje ganador
            auxMsj = "El puntaje de la mesa es de: " + to_string(puntajeMesa) +
                     "\nEl puntaje del jugador es de: " + to_string(puntajeJugador) +
                     "\nJugador " + to_string(jugador1->getId()) + " ha perdido por tener menos puntos que la mesa :(" +
                     "\nSu nuevo monto es de: " + to_string(jugador1->getMonto()) + "\n";
            //  Enviar mensaje de victoria por la PIPE2
            enviarMensaje(fd2, pid, mutex, msg, auxMsj);
        }
    }

    // Close the named pipes and the semaphore
    close(fd1);
    close(fd2);
    sem_close(mutex);

    return 0;
}