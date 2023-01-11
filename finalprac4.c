#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#define MAX 256

void manejador(int signum);

int tuberia_emisor[2];
int tuberia_receptor[2];
int pid;
char mensaje[MAX];
char mensaje2[] = "Listo!";

int main(int argc, char const *argv[])
{
    int status;
    // Creación de tuberías
    if (pipe(tuberia_emisor) == -1)
    {
        perror("pipe");
        exit(-1);
    }
    if (pipe(tuberia_receptor) == -1)
    {
        perror("pipe");
        exit(-1);
    }
    // Creación de proceso hijo
    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(-1);
    }
    else if (pid == 0)
    { /*Código del proceso hijo*/

        while (1)
        {
            signal(SIGUSR1, manejador);
        }
        close(tuberia_emisor[0]);
        close(tuberia_emisor[1]);
        close(tuberia_receptor[0]);
        close(tuberia_receptor[1]);
        exit(0);
    }
    else
    { /*Código del proceso padre*/

        while (1)
        {
            printf("Proceso emisor. Introduce el mensaje:\n");
            fgets(mensaje, sizeof(mensaje), stdin);
            if (strcmp(mensaje, "FIN\n") == 0)
            {
                kill(SIGTERM, SIGUSR1);
                break;
            }
            write(tuberia_emisor[1], mensaje, strlen(mensaje) + 1);
            kill(pid, SIGUSR1);
            signal(SIGUSR2, manejador);
            pause();
        }
        close(tuberia_emisor[1]);
        close(tuberia_emisor[0]);
        close(tuberia_receptor[1]);
        close(tuberia_receptor[0]);
        exit(0);
    }
}

void manejador(int signum)
{
    // Si la señal es SIGUSR1 entonces
    if (signum == SIGUSR1)
    {
        read(tuberia_emisor[0], mensaje, MAX);
        printf("\nProceso receptor. MENSAJE:%s\n", mensaje);
        write(tuberia_receptor[1], mensaje2, strlen(mensaje2) + 1);
        sleep(3);
        kill(getppid(), SIGUSR2);
    }
    // si la señal es SIGUSR1 entonces
    if (signum == SIGUSR2)
    {
        read(tuberia_receptor[0], mensaje2, 7);
        printf("Proceso emisor. MENSAJE RECIBIDO:%s\n", mensaje2);
    }
}
