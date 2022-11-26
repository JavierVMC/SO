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

#define MAX_NUM_REST 100

typedef struct cliente
{
    int x;
    int y;
    bool served;
    pthread_t pid_r;
    pthread_t pid;
} Cliente;

typedef struct restaurant
{
    int x;
    int y;
    pthread_t pid;
} Restaurante;

typedef struct motorizado
{
    int x;
    int y;
    pthread_t pid;
} Motorizado;

pthread_t tid[MAX_NUM_REST];
int counter;
pthread_mutex_t lock;

Restaurante restaurantes[MAX_NUM_REST];

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

/* Crea al restaurante, lo pone en un lugar aleatorio del mapa y lo conecta con el cliente y el motorizado */
void *manageRestaurant(void *arg)
{
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);
    const int *num_restaurantes = readSharedInt(NUM_REST);
    const int *num_motorizados = readSharedInt(NUM_MOT);
    char espacio;
    int i, j, num_r, num_m;

    /* Distribucion aleatoria de restaurante */
    pthread_mutex_lock(&lock);

    char *matriz = readMatrix();
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
            pthread_t pid = pthread_self();
            printf("pid = %ld\n", pid);
            Restaurante r = {x, y, pid};
            restaurantes[counter] = r;
        }
    } while (espacio != ' ');

    counter += 1;
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

    do
    {
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

    const int *num_restaurantes = readSharedInt(NUM_REST);

    while (i < *num_restaurantes)
    {
        error = pthread_create(&(tid[i]), NULL, &manageRestaurant, NULL);
        if (error != 0)
        {
            printf("\nThread cannot be created : [%s]", strerror(error));
        }
        i++;
    }

    for (i = 0; i < *num_restaurantes; i++)
    {
        int error = pthread_join(tid[i], NULL);
        fprintf(stderr, "Thread [%ld] exit with %d\n", tid[i], error);
    }

    pthread_mutex_destroy(&lock);

    for (i = 0; i < *num_restaurantes; i++)
    {
        printf("Restaurante [%ld] (%d, %d)\n", restaurantes[i].pid, restaurantes[i].x, restaurantes[i].y);
    }

    return 0;
}