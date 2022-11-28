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
#include "queue.h"
#include "myFunctions.h"

pthread_t tid[MAX_NUM_REST];
int counter;
pthread_mutex_t lock;

Restaurante restaurantes[MAX_NUM_REST];

char message_to_c[LEN_MESSAGE] = "Ya hice tu comida!";

void *manageRestaurant(void *arg);

int main()
{
    int i = 0;
    int error;
    const int *num_restaurantes = readSharedInt(NUM_REST);

    /* Hilos */
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\nMutex init has failed\n");
        return 1;
    }

    /* Creacion de hilos por cada restaurante */
    for (i = 0; i < *num_restaurantes; i++)
    {
        error = pthread_create(&(tid[i]), NULL, &manageRestaurant, NULL);
        if (error != 0)
        {
            printf("\nThread cannot be created : [%s]", strerror(error));
        }
    }

    /* Esperando a los hilos de los restaurantes */
    for (i = 0; i < *num_restaurantes; i++)
    {
        int error = pthread_join(tid[i], NULL);
        if (error != 0)
        {
            fprintf(stderr, "Error en Thread [%ld] exit with %d\n", tid[i], error);
        }
    }

    pthread_mutex_destroy(&lock);

    printSharedMatrix();
    printSharedRestaurants();
}

/* Crea al restaurante, lo pone en un lugar aleatorio del mapa y lo conecta con el cliente y el motorizado */
void *manageRestaurant(void *arg)
{
    int num_r;
    pthread_t pid = pthread_self();

    pthread_mutex_lock(&lock);

    createRestaurant(pid, &num_r, &counter, restaurantes);
    counter += 1;

    pthread_mutex_unlock(&lock);

    int vuelta = 0;
    /* Lee el mensaje en el pipe con el cliente */
    while (1)
    {
        Restaurante *ptr = editSharedRestaurants();
        pthread_t currentPipe = INT_MIN;
        pthread_t currentClient = INT_MIN;
        char pipePath[LEN_MESSAGE] = "";
        while (currentPipe == INT_MIN)
        {
            pthread_mutex_lock(&lock);
            currentPipe = dequeue(&ptr[num_r].colaNumPipes);
            ptr = editSharedRestaurants();
            pthread_mutex_unlock(&lock);
            sleep(1);
        }
        while (currentClient == INT_MIN)
        {
            pthread_mutex_lock(&lock);
            currentClient = dequeue(&ptr[num_r].colaClientes);
            ptr = editSharedRestaurants();
            pthread_mutex_unlock(&lock);
            sleep(1);
        }

        createPipePath(pipePath, currentPipe);
        int fd;
        int status;
        /* Espera el mensaje del cliente */

        char message_from_c[LEN_MESSAGE];
        fd = open(pipePath, O_RDONLY);
        read(fd, message_from_c, LEN_MESSAGE);
        printf("Message from client [%ld] to restaurant [%ld]: %s\n", currentClient, pid, message_from_c);
        close(fd);

        /* Preparando comida */
        unsigned int seed = time(NULL);
        int tiempoRandom = (rand_r(&seed) % 3) + 1;
        printf("[%ld] Preparando comida para cliente [%ld]\n", pid, currentClient);
        sleep(tiempoRandom);
        printf("[%ld] Comida terminada para cliente [%ld]\n", pid, currentClient);
        /* Escribe un mensaje al cliente */
        fd = open(pipePath, O_WRONLY);
        write(fd, message_to_c, LEN_MESSAGE);
        close(fd);
        printf("[%ld] Mensaje enviado al cliente [%ld]: %s\n", pid, currentClient, message_to_c);

        vuelta++;
    }

    return NULL;
}