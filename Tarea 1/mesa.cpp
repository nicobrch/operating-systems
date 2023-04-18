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
    Jugador jugadores[cantidadJugadores];

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

    //  Crear semaforos
    for (i=0; i < cantidadJugadores; i++)
    {
        string path = "mutex" + to_string(i);
        mutex[i] = sem_open(path.c_str(), O_CREAT, 0666, 1);
    }

    //  Esperar que los jugadores se conecten
    cout << "Esperando jugadores..." << endl;

    for (i=0; i<cantidadJugadores; i++){
        //  Leer mensaje desde la PIPE1
        msg = leerMensajeInicial(fd1[i], msg);

        //  Crear jugador que se ha conectado
        jugadores[i].setId(msg.process_id);
        jugadores[i].setMonto(1000);
        jugadores[i].agregarCarta();
        jugadores[i].agregarCarta();

        //  Enviar mensaje a la PIPE2
        enviarMensaje(fd2[i], pid, mutex[i], msg,
    "Hola Jugador " + to_string(jugadores[i].getId()) + "! Bienvenido al juego del 21" +
            "\nUsted cuenta con un monto total de " + to_string(jugadores[i].getMonto()) +
            ".\nPor favor indique cuánto desea apostar.");

        //  Leer mensaje desde la PIPE1
        msg = leerMensaje(fd1[i], mutex[i], msg);

        //  Modificar monto
        int apuesta = charArrayToInt(msg);
        auxMsj = jugadores[i].imprimirCartas();

        //  Bucle que consulta por si quiere agregar o no cartas
        while(jugadores[i].getMonto() > 0)
        {
            //  Variables
            int puntajeMesa = mesa->getPuntaje(), puntajeJugador = jugadores[i].getSumaCartas();

            //  Enviar sus cartas al jugador por la PIPE2
            enviarMensaje(fd2[i], pid, mutex[i], msg, auxMsj);

            //  Leer mensaje desde la PIPE1
            msg = leerMensaje(fd1[i], mutex[i], msg);

            //  Opcion ingresada por el jugador parseada a int
            int opcion = charArrayToInt(msg);

            if (opcion == 1)
            {
                //  Desea una carta adicional.
                jugadores[i].agregarCarta();
                if (jugadores[i].getSumaCartas() > 21)
                {
                    //  Modificar monto
                    jugadores[i].setMonto(jugadores->getMonto() - apuesta);
                    //  Reiniciar cartas para nuevo juego
                    jugadores[i].nuevoJuego();
                    mesa->nuevoPuntaje();
                    //  Mensaje ganador
                    auxMsj = "El puntaje de la mesa es de: " + to_string(puntajeMesa) +
                             "\nEl puntaje del jugador es de: " + to_string(puntajeJugador) +
                             "\n> Jugador " + to_string(jugadores[i].getId()) + " ha perdido por pasarse del 21 :(" +
                             "\nSu nuevo monto es de: " + to_string(jugadores[i].getMonto()) +
                             "\n[---- NUEVO JUEGO ----]\n";
                    auxMsj += jugadores[i].imprimirCartas();
                }
                else
                {
                    auxMsj = jugadores[i].imprimirCartas();
                }
            }
            else
            {
                if (puntajeJugador > puntajeMesa && puntajeJugador <= 21)
                {
                    //  Ganó el juego
                    //  Sumar lo apostado*2 a su monto total
                    jugadores[i].setMonto(jugadores[i].getMonto() + (apuesta * 2));
                    //  Reiniciar cartas para nuevo juego
                    jugadores[i].nuevoJuego();
                    mesa->nuevoPuntaje();
                    //  Mensaje ganador
                    auxMsj = "El puntaje de la mesa es de: " + to_string(puntajeMesa) +
                             "\nEl puntaje del jugador es de: " + to_string(puntajeJugador) +
                             "\n> Jugador " + to_string(jugadores[i].getId()) + " ha ganado!" +
                             "\nSu nuevo monto es de: " + to_string(jugadores[i].getMonto()) +
                             "\n[---- NUEVO JUEGO ----]\n";
                    auxMsj += jugadores[i].imprimirCartas();
                }
                else
                {
                    //  Sumar lo apostado a su monto total
                    jugadores[i].setMonto(jugadores[i].getMonto() - apuesta);
                    //  Reiniciar cartas para nuevo juego
                    jugadores[i].nuevoJuego();
                    mesa->nuevoPuntaje();
                    //  Mensaje ganador
                    auxMsj = "El puntaje de la mesa es de: " + to_string(puntajeMesa) +
                             "\nEl puntaje del jugador es de: " + to_string(puntajeJugador) +
                             "\n>Jugador " + to_string(jugadores[i].getId()) + " ha perdido por tener menos puntos que la mesa :(" +
                             "\nSu nuevo monto es de: " + to_string(jugadores[i].getMonto()) +
                             "\n[---- NUEVO JUEGO ----]\n";
                    auxMsj += jugadores[i].imprimirCartas();
                }
            }

            if (jugadores[i].getMonto() < 0 || opcion == 9)
            {
                auxMsj = to_string(9);
                enviarMensaje(fd2[i], pid, mutex[i], msg, auxMsj);
                break;
            }
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