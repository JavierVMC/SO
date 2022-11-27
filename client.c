#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include "myFunctions.h"

pthread_t tid[MAX_CLIENTS];
int counter;
pthread_mutex_t lock;

Cliente clientes[MAX_CLIENTS];
Restaurante *restaurantes;
bool continuar = true;

char pipePaths[MAX_CLIENTS][LEN_MESSAGE];

char message_to_r[LEN_MESSAGE] = "Quiero comida!";
char message_to_m[LEN_MESSAGE] = "Quiero que me traigas mi comida!";

void *manageClient(void *arg);

int main()
{
    restaurantes = editSharedRestaurants();

    /* Creacion de hilos */
    int i = 0;
    int error;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\nMutex init has failed\n");
        return 1;
    }

    const int *intervalo_ms = readSharedInt(INTERVALO_MS);

    while (i < MAX_CLIENTS && continuar)
    {
        msleep(*intervalo_ms);
        srand(time(0));
        if (rand() % 10 < 5)
        {
            char pipePath[LEN_MESSAGE] = "";
            char number[MAX_DIGITS] = "";
            sprintf(number, "%d", i);
            strcat(pipePath, "/tmp/mypipe");
            strcat(pipePath, number);
            mkfifo(pipePath, 0666);
            strcpy(pipePaths[i], pipePath);
            error = pthread_create(&(tid[i]), NULL, &manageClient, pipePaths[i]);
            if (error != 0)
            {
                printf("\nThread cannot be created : [%s]", strerror(error));
            }
            i++;
        }
    }

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        int error = pthread_join(tid[i], NULL);
        fprintf(stderr, "Thread [%ld] exit with %d\n", tid[i], error);
    }

    pthread_mutex_destroy(&lock);

    printSharedMatrix();
    printSharedRestaurants();

    return 0;
}

/* Crea al cliente, lo pone en un lugar aleatorio del mapa y lo conecta con el restaurante y el motorizado */
void *manageClient(void *arg)
{
    char *pipePath = (char *)arg; // File descriptor del pipe con el restaurante
    int result_r;                 // Resultado de escribir en el pipe del restaurante
    char espacio;
    int i, j, num_r, num_m, num_c;

    const int *dimension_matriz = readSharedInt(DIM_MATRIX);
    const int *num_restaurantes = readSharedInt(NUM_REST);
    const int *num_motorizados = readSharedInt(NUM_MOT);

    /* Distribucion aleatoria de cliente */
    pthread_mutex_lock(&lock);

    pthread_t pid = pthread_self();
    char *matriz = readSharedMatrix();

    do
    {
        unsigned int seed = time(NULL);
        i = rand_r(&seed) % *dimension_matriz;
        j = rand_r(&seed) % *dimension_matriz;
        num_r = rand_r(&seed) % *num_restaurantes;
        espacio = *(matriz + i * *dimension_matriz + j);
        if (espacio == ' ')
        {
            *(matriz + i * *dimension_matriz + j) = 'c';
            int x = j - (*dimension_matriz / 2);
            int y = (*dimension_matriz / 2) - i;
            Cliente c = {x, y, false, "", restaurantes[num_r].pid, pid};
            restaurantes[num_r].pid_c, pid;
            strcpy(restaurantes[num_r].pipePath, pipePath);
            strcpy(c.pipePath, pipePath);
            clientes[counter] = c;
            printf("Cliente [%ld] (%d,%d) {%s} -> Restaurante [%ld]\n", clientes[counter].pid, clientes[counter].x, clientes[counter].y, clientes[counter].pipePath, clientes[counter].pid_r);
        }
    } while (espacio != ' ');
    counter += 1;
    pthread_mutex_unlock(&lock);

    /* PIPE FIFO */
    int fd;
    char message_from_r[LEN_MESSAGE];
    /* Escribe un mensaje al restaurante */
    fd = open(pipePath, O_WRONLY);
    write(fd, message_to_r, LEN_MESSAGE);
    close(fd);
    int status;
    while (status == 0)
    {
        /* Lee un mensaje del restaurante */
        fd = open(pipePath, O_RDONLY);
        status = read(fd, message_from_r, LEN_MESSAGE);
        if (status != -1 && status != 0)
        {
            printf("Message from restaurant [%ld] to client [%ld]: %s\n", restaurantes[num_r].pid, pid, message_from_r);
        }
        close(fd);
    }
    return NULL;
}