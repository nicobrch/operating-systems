#include "utils.h"

int main() {
    // Variables
    int pid = getpid(), cantidadJugadores, i;
    vector<int> cartas;
    message msg{};
    string auxMsj;
    auto mesa = new Mesa;

    cout << "Bienvenido al juego del 21.\nIngrese cuántos jugadores desea tener:" << endl;
    cin >> cantidadJugadores;

    int fd1[cantidadJugadores], fd2[cantidadJugadores];
    sem_t *mutex[cantidadJugadores];

    //  Crear pipes
    for (i=0; i < (cantidadJugadores*2); i++)
    {
        string path = "fifo" + to_string(i);
        mkfifo(path.c_str(), 0666);
    }

    // Abrir pipes
    int contador = 0;
    for (i=0; i < (cantidadJugadores*2); i=i+2)
    {
        string path = "fifo" + to_string(i);
        string path2 = "fifo" + to_string(i+1);
        fd1[contador] = open(path.c_str(), O_RDWR);
        fd2[contador] = open(path2.c_str(), O_RDWR);
        contador++;
    }

    // Crear semaforos
    for (i=0; i < cantidadJugadores; i++)
    {
        string path = "mutex" + to_string(i);
        mutex[i] = sem_open(path.c_str(), O_CREAT, 0666, 1);
    }

    cout << "Esperando jugadores..." << endl;

    //  Leer mensaje desde la PIPE1
    msg = leerMensajeInicial(fd1[0], msg);

    //  Crear jugador que se ha conectado
    auto jugador1 = new Jugador(msg.process_id, 1000);

    cout << "fd2[0]: " << fd2[0] << endl;
    //  Enviar mensaje a la PIPE2
    enviarMensaje(fd2[0], pid, mutex[0], msg, "Hola Jugador " + to_string(jugador1->getId()) + "! Bienvenido al" +
        " juego del 21.\nUsted cuenta con un monto total de " + to_string(jugador1->getMonto()) + ". Por favor " +
        "indique cuánto desea apostar.");

    //  Leer mensaje desde la PIPE1
    msg = leerMensaje(fd1[0], mutex[0], msg);

    //  Modificar monto
    int apuesta = charArrayToInt(msg);
    auxMsj = jugador1->imprimirCartas();

    //  Enviar cartas sus cartas al jugador por la PIPE2
    enviarMensaje(fd2[0], pid, mutex[0], msg, auxMsj);

    //  Leer mensaje desde la PIPE1
    msg = leerMensaje(fd1[0], mutex[0], msg);

    int opcion = charArrayToInt(msg);
    if (opcion == 1)
    {
        //  Desea una carta adicional.
        jugador1->agregarCarta();
        auxMsj = jugador1->imprimirCartas();
        //  Enviar cartas sus cartas al jugador por la PIPE2
        enviarMensaje(fd2[0], pid, mutex[0], msg, auxMsj);
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
            enviarMensaje(fd2[0], pid, mutex[0], msg, auxMsj);
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
            enviarMensaje(fd2[0], pid, mutex[0], msg, auxMsj);
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
            enviarMensaje(fd2[0], pid, mutex[0], msg, auxMsj);
        }
    }

    // Cerrar las pipes y los semaforos
    for (i=0; i<cantidadJugadores; i++)
    {
        close(fd1[i]);
        close(fd2[i]);
        sem_close(mutex[i]);
    }

    return 0;
}