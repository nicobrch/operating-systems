#include "utils.h"

int main() {
    // Variables
    int fd1, fd2, pid = getpid();
    sem_t *mutex;
    message msg{};
    string auxMsj;

    // Abrir pipes
    fd1 = open("fifo0", O_WRONLY);
    fd2 = open("fifo1", O_RDONLY);

    mutex = sem_open("mutex0", O_CREAT, 0666, 1);

    //  Enviar mensaje a la PIPE1 'Jugador conectado'
    enviarMensaje(fd1, pid, mutex, msg, "Jugador se ha conectado");

    //  Leer mensaje desde la PIPE2 'Cuanto dinero desea apostar'
    msg = leerMensaje(fd2, mutex, msg);

    //  Enviar mensaje CIN a la PIPE1 'Cantidad a apostar'
    enviarMensajeCIN(fd1, pid, mutex, msg);

    //  Leer mensaje desde la PIPE 2 'Cartas obtenidas'
    msg = leerMensaje(fd2, mutex, msg);

    while (true)
    {
        //  Enviar mensaje CIN a la PIPE 1 'Ingresar 1 para mas cartas o 0 para jugar'
        enviarMensajeCIN(fd1, pid, mutex, msg);

        //  Resultado de la última opción
        msg = leerMensaje(fd2, mutex, msg);

        int opcion = charArrayToInt(msg);
        if (opcion == 9)
        {
            cout << "Se ha terminado el programa." << endl;
            break;
        }
    }

    close(fd1);
    close(fd2);
    sem_close(mutex);

    exit(0);
}