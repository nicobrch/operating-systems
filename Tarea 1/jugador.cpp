#include "utils.h"

int main() {
    // Variables
    int fd1, fd2, pid = getpid();
    long n;
    sem_t *mutex;
    message msg{};
    string auxMsj;

    // Abrir pipes
    fd1 = open(FIFO1, O_WRONLY);
    fd2 = open(FIFO2, O_RDONLY);

    mutex = sem_open("mutex", O_CREAT, 0666, 1);

    //  Enviar mensaje a la PIPE1
    enviarMensaje(fd1, pid, mutex, msg, "Jugador se ha conectado");

    //  Leer mensaje desde la PIPE2
    msg = leerMensaje(fd2, mutex, msg);

    //  Enviar mensaje CIN a la PIPE1
    enviarMensajeCIN(fd1, pid, mutex, msg);

    //  Leer mensaje desde la PIPE 2
    msg = leerMensaje(fd2, mutex, msg);

    //  Enviar mensaje CIN a la PIPE 1
    enviarMensajeCIN(fd1, pid, mutex, msg);

    leerMensaje(fd2, mutex, msg);

    close(fd1);
    close(fd2);
    sem_close(mutex);

    return 0;
}