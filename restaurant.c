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
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);
    const int *num_restaurantes = readSharedInt(NUM_REST);
    const int *num_motorizados = readSharedInt(NUM_MOT);
    int i, j, num_r, num_m;
    char espacio;
    char pipePath[LEN_MESSAGE] = "";

    /* Distribucion aleatoria de restaurante */
    pthread_mutex_lock(&lock);

    char *matriz = readSharedMatrix();
    pthread_t pid = pthread_self();

    do
    {
        unsigned int seed = time(NULL);
        i = rand_r(&seed) % *dimension_matriz;
        j = rand_r(&seed) % *dimension_matriz;
        espacio = *(matriz + i * *dimension_matriz + j);
        if (espacio == ' ')
        {
            *(matriz + i * *dimension_matriz + j) = 'r';
            int x = j - (*dimension_matriz / 2);
            int y = (*dimension_matriz / 2) - i;
            Restaurante r = {x, y, "", pid, pid};
            restaurantes[counter] = r;
            /* Guardar los restaurantes en memoria compartida */
            Restaurante *ptr = createSharedRestaurants();
            for (i = 0; i < *num_restaurantes; i++)
            {
                ptr[i].x = restaurantes[i].x;
                ptr[i].y = restaurantes[i].y;
                ptr[i].pid_c = restaurantes[i].pid_c;
                ptr[i].pid = restaurantes[i].pid;
                strcpy(ptr[i].pipePath, restaurantes[i].pipePath);
            }
            num_r = counter;
            printf("Restaurante [%ld] (%d,%d) {%s}\n", ptr[counter].pid, ptr[counter].x, ptr[counter].y, ptr[counter].pipePath);
        }
    } while (espacio != ' ');

    counter += 1;
    pthread_mutex_unlock(&lock);

    /* Lee el mensaje en el pipe con el cliente */
    strcpy(pipePath, restaurantes[num_r].pipePath);
    Restaurante *ptr;
    while (strcmp(pipePath, "") == 0)
    {
        ptr = readSharedRestaurants();
        strcpy(pipePath, ptr[num_r].pipePath);
        sleep(1);
    }
    /* PIPE FIFO */
    int fd;
    int status;
    while (status == 0)
    {
        char message_from_c[LEN_MESSAGE];
        fd = open(pipePath, O_RDONLY);
        /* Lee un mensaje del cliente */
        status = read(fd, message_from_c, LEN_MESSAGE);
        if (status != -1 && status != 0)
        {
            printf("Message from client [%ld] to restaurant [%ld]: %s\n", ptr[num_r].pid_c, pid, message_from_c);
        }
        close(fd);
    }

    /* Lee un mensaje del restaurante */
    fd = open(pipePath, O_WRONLY);
    write(fd, message_to_c, LEN_MESSAGE);
    close(fd);

    return NULL;
}