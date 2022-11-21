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

#define SMOBJ_NAME "/matriz"
#define DIM_MATRIX "/dimMatriz"

#define MAX_CLIENTS 10

struct cliente
{
    int x;
    int y;
    int number;
    bool served;
};

pthread_t tid[MAX_CLIENTS];
int counter;
pthread_mutex_t lock;

struct cliente clientes[MAX_CLIENTS];
bool continuar = true;

/* Lee la matriz de la memoria compartida */
char *readMatrix()
{
    int fd;
    char *ptr;
    struct stat shmobj_st;
    fd = shm_open(SMOBJ_NAME, O_CREAT | O_RDWR, 00700);
    if (fd == -1)
    {
        printf("Error file descriptor\n");
        exit(1);
    }
    if (fstat(fd, &shmobj_st) == -1)
    {
        printf("Error fstat\n");
        exit(1);
    }
    ptr = mmap(NULL, shmobj_st.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    close(fd);
    return ptr;
}

/* Lee la dimension de la matriz de la memoria compartida */
const int *readDimensionOfMatrix()
{
    int fd;
    const int *ptr;
    struct stat shmobj_st;
    fd = shm_open(DIM_MATRIX, O_RDONLY, 00400);
    if (fd == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if (fstat(fd, &shmobj_st) == -1)
    {
        printf("Error fstat\n");
        exit(1);
    }

    ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in read process: %s\n", strerror(errno));
        exit(1);
    }
    return ptr;
}

/* Agrega un cliente en la matriz */
void generateClient()
{
    char *matriz = readMatrix();
    const int *dimension_matriz = readDimensionOfMatrix();
    char espacio;
    int i, j;

    /* Distribucion aleatoria de cliente */
    do
    {
        unsigned int seed = time(NULL);
        i = rand_r(&seed) % *dimension_matriz;
        j = rand_r(&seed) % *dimension_matriz;
        espacio = *(matriz + i * *dimension_matriz + j);
        if (espacio == ' ')
        {
            *(matriz + i * *dimension_matriz + j) = 'c';
            struct cliente c = {j - (*dimension_matriz / 2), (*dimension_matriz / 2) - i, counter};
            clientes[counter] = c;
        }
    } while (espacio != ' ');

    /* Imprimir matriz */
    for (i = 0; i < *dimension_matriz; i++)
    {
        for (j = 0; j < *dimension_matriz; j++)
        {
            printf("%c ", *(matriz + i * *dimension_matriz + j));
        }
        printf("\n");
    }
}

/* Lo que haran los hilos creados */
void *trythis(void *arg)
{
    pthread_mutex_lock(&lock);

    counter += 1;
    printf("Matriz from client [%d]\n", counter);
    generateClient();

    pthread_mutex_unlock(&lock);

    return NULL;
}

int main()
{
    char *matriz = readMatrix();
    const int *dimension_matriz = readDimensionOfMatrix();

    /* Imprimir matriz en memoria compartida */
    int i, j;
    for (i = 0; i < *dimension_matriz; i++)
    {
        for (j = 0; j < *dimension_matriz; j++)
        {
            printf("%c ", *(matriz + i * *dimension_matriz + j));
        }
        printf("\n");
    }

    /* Creacion de hilos */
    i = 0;
    int error;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\nMutex init has failed\n");
        return 1;
    }

    while (i < MAX_CLIENTS && continuar)
    {
        sleep(1);
        error = pthread_create(&(tid[i]), NULL, &trythis, &(tid[i]));
        if (error != 0)
        {
            printf("\nThread cannot be created : [%s]", strerror(error));
        }
        i++;
    }

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    return 0;
}