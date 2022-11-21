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
#define NUM_REST "/numRestaurantes"
#define NUM_MOT "/numMotorizados"
#define INTERVALO_MS "/intervaloMs"

#define REST_NAME "/restaurantes"
#define MOT_NAME "/motorizados"

#define MAX_CLIENTS 10

struct cliente
{
    int x;
    int y;
    int number;
    int num_r;
    bool served;
};

struct restaurante
{
    int x;
    int y;
    int number;
};

struct motorizado
{
    int x;
    int y;
    int number;
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

/* Lee un entero de la memoria compartida */
const int *readSharedInt(const char *name)
{
    int fd;
    const int *ptr;
    struct stat shmobj_st;
    fd = shm_open(name, O_RDONLY, 00400);
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
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);
    const int *num_restaurantes = readSharedInt(NUM_REST);
    const int *num_motorizados = readSharedInt(NUM_MOT);
    char espacio;
    int i, j, num_r, num_m;

    /* Distribucion aleatoria de cliente */
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
            struct cliente c = {j - (*dimension_matriz / 2), (*dimension_matriz / 2) - i, counter, num_r};
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

/* Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main()
{
    char *matriz = readMatrix();
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);

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

    const int *intervalo_ms = readSharedInt(INTERVALO_MS);

    while (i < MAX_CLIENTS && continuar)
    {
        msleep(*intervalo_ms);
        srand(time(0));
        if(rand() % 10 < 5){
            error = pthread_create(&(tid[i]), NULL, &trythis, &(tid[i]));
            if (error != 0)
            {
                printf("\nThread cannot be created : [%s]", strerror(error));
            }
            i++;
        }
    }

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    for(i = 0; i< MAX_CLIENTS; i++){
        printf("Cliente [%d] -> Restaurante [%d]\n", clientes[i].number, clientes[i].num_r);
    }

    return 0;
}