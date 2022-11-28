#include "myFunctions.h"
#include "queue.h"
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

/* Imprime la ayuda del programa principal */
void print_help(char *command)
{
    printf("chasquiEats inicializa la simulacion del servicio de pedidos.\n");
    printf("uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

/* Creacion de memoria compartida para almacenar la matriz */
char *createSharedMatrix()
{
    int fd;
    char *ptr;
    fd = shm_open(MATRIX_NAME, O_CREAT | O_RDWR, 00700);
    if (fd == -1)
    {
        printf("Error file descriptor\n");
        exit(1);
    }
    if (ftruncate(fd, sizeof(SIZE_OF_SMOBJ)) == -1)
    {
        printf("Error shared memory cannot be resized\n");
    }
    ptr = mmap(NULL, sizeof(SIZE_OF_SMOBJ), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    close(fd);
    return ptr;
}

/* Lee la matriz de la memoria compartida */
char *readSharedMatrix()
{
    int fd;
    char *ptr;
    struct stat shmobj_st;
    fd = shm_open(MATRIX_NAME, O_CREAT | O_RDWR, 00700);
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

/* Creacion de memoria compartida para almacenar un entero */
void createSharedInt(const int num, const char *name)
{
    int fd;
    int *ptr;
    fd = shm_open(name, O_CREAT | O_RDWR, 00700);
    if (fd == -1)
    {
        printf("Error file descriptor\n");
        exit(1);
    }
    if (ftruncate(fd, sizeof(int)) == -1)
    {
        printf("Error shared memory cannot be resized\n");
    }
    ptr = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    memcpy(ptr, &num, sizeof(int));
    close(fd);
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
    close(fd);
    return ptr;
}

/* Creacion de memoria compartida para almacenar el arreglo de restaurantes */
Restaurante *createSharedRestaurants()
{
    int fd;
    Restaurante *ptr;
    fd = shm_open(REST_NAME, O_CREAT | O_RDWR, 00700);
    if (fd == -1)
    {
        printf("Error file descriptor\n");
        exit(1);
    }
    if (ftruncate(fd, sizeof(SIZE_OF_SMOBJ)) == -1)
    {
        printf("Error shared memory cannot be resized\n");
    }
    ptr = mmap(NULL, sizeof(SIZE_OF_SMOBJ), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    close(fd);
    return ptr;
}

/* Lee el arreglo de restaurantes de la memoria compartida */
Restaurante *readSharedRestaurants()
{
    int fd;
    Restaurante *ptr;
    struct stat shmobj_st;
    fd = shm_open(REST_NAME, O_RDONLY, 00400);
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
    close(fd);
    return ptr;
}

/* Lee el arreglo de restaurantes de la memoria compartida */
Restaurante *editSharedRestaurants()
{
    int fd;
    Restaurante *ptr;
    struct stat shmobj_st;
    fd = shm_open(REST_NAME, O_CREAT | O_RDWR, 00700);
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

    ptr = mmap(NULL, shmobj_st.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in read process: %s\n", strerror(errno));
        exit(1);
    }
    close(fd);
    return ptr;
}

/* Creacion de memoria compartida para almacenar el arreglo de restaurantes */
Cliente *createSharedClients()
{
    int fd;
    Cliente *ptr;
    fd = shm_open(CLT_NAME, O_CREAT | O_RDWR, 00700);
    if (fd == -1)
    {
        printf("Error file descriptor\n");
        exit(1);
    }
    if (ftruncate(fd, sizeof(SIZE_OF_SMOBJ)) == -1)
    {
        printf("Error shared memory cannot be resized\n");
    }
    ptr = mmap(NULL, sizeof(SIZE_OF_SMOBJ), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    close(fd);
    return ptr;
}

/* Lee el arreglo de restaurantes de la memoria compartida */
Cliente *readSharedClients()
{
    int fd;
    Cliente *ptr;
    struct stat shmobj_st;
    fd = shm_open(CLT_NAME, O_RDONLY, 00400);
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
    close(fd);
    return ptr;
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

/* Imprimir matriz de la memoria compartida */
void printSharedMatrix()
{
    char *matriz = readSharedMatrix();
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);

    for (int i = 0; i < *dimension_matriz; i++)
    {
        for (int j = 0; j < *dimension_matriz; j++)
        {
            printf("%c ", *(matriz + i * *dimension_matriz + j));
        }
        printf("\n");
    }
}

/* Imprime el arreglo de restaurantes de la memoria compartida */
void printSharedRestaurants()
{
    printf("Memoria compartida [Restaurantes]\n");
    Restaurante *ptr_read = readSharedRestaurants();
    const int *num_restaurantes = readSharedInt(NUM_REST);
    for (int i = 0; i < *num_restaurantes; i++)
    {
        printf("Restaurante [%ld] (%d,%d) -> ", ptr_read[i].pid, ptr_read[i].x, ptr_read[i].y);
        printQueueArray(&ptr_read[i].colaClientes);
    }
}

/* Imprime el arreglo de restaurantes de la memoria compartida */
void printSharedClients()
{
    printf("Memoria compartida [Clientes]\n");
    Cliente *ptr_read = readSharedClients();
    const int *num_clientes = readSharedInt(NUM_CLT);
    for (int i = 0; i < *num_clientes; i++)
    {
        printf("Cliente [%ld] (%d,%d) {%s} -> Restaurante [%ld]\n", ptr_read[i].pid, ptr_read[i].x, ptr_read[i].y, ptr_read[i].pipePath, ptr_read[i].pid_r);
    }
}

/* Guarda el pipePath con el numero i en el puntero pipePath */
void createPipePath(char *pipePath, int i)
{
    char number[MAX_DIGITS] = "";
    sprintf(number, "%d", i);
    strcat(pipePath, "/tmp/mypipe");
    strcat(pipePath, number);
}

/* Imprime los elementos de la cola sin sacarlos */
void printQueue(Queue *queue)
{
    printf("Queue:");
    for (int i = 0; i < queue->size; i++)
    {
        printf(" %ld |", queue->array[i]);
    }
    printf("\n");
}

/* Imprime los elementos del array de la cola */
void printQueueArray(Queue *queue)
{
    printf("Queue:");
    for (int i = 0; i < MAX_QUEUE_CAPACITY; i++)
    {
        printf(" %ld |", queue->array[i]);
    }
    printf("\n");
}

/* Imprime informacion sobre el cliente y el restaurante al que ordeno */
void printClientInfo(Cliente *cliente, Restaurante *restaurante)
{
    printf("Cliente [%ld] (%d,%d) {%s} -> Restaurante [%ld] ", cliente->pid, cliente->x, cliente->y, cliente->pipePath, cliente->pid_r);
    printQueue(&restaurante->colaClientes);
}

/* Imprime informacion sobre el restaurante */
void printRestaurantInfo(Restaurante *restaurante)
{
    printf("Restaurante [%ld] (%d,%d) ", restaurante->pid, restaurante->x, restaurante->y);
    printQueue(&restaurante->colaClientes);
}

/* Crea un restaurante en una posicion aleatoria de la matriz */
void createRestaurant(pthread_t pid, int *num_r, int *counter, Restaurante *restaurantes)
{
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);
    const int *num_restaurantes = readSharedInt(NUM_REST);
    const int *num_motorizados = readSharedInt(NUM_MOT);
    int i, j;
    char espacio;
    Queue colaClientes = createQueue();
    Queue colaNumPipes = createQueue();
    char *matriz = readSharedMatrix();
    /* Distribucion aleatoria de restaurante */
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
            Restaurante r = {x, y, pid, colaClientes, colaNumPipes};
            restaurantes[*counter] = r;
            /* Guardar los restaurantes en memoria compartida */
            Restaurante *ptr = createSharedRestaurants();
            for (i = 0; i < *num_restaurantes; i++)
            {
                ptr[i].x = restaurantes[i].x;
                ptr[i].y = restaurantes[i].y;
                ptr[i].pid = restaurantes[i].pid;
                ptr[i].colaClientes = restaurantes[i].colaClientes;
                ptr[i].colaNumPipes = restaurantes[i].colaNumPipes;
            }
            *num_r = *counter;
            printRestaurantInfo(&ptr[*counter]);
        }
    } while (espacio != ' ');
}

/* Crea un cliente en una posicion aleatoria de la matriz y lo hace ordenar a un restaurante aleatorio */
void createClient(pthread_t pid, int *num_r, int *counter, char *pipePath, Restaurante *restaurantes, Cliente *clientes)
{
    const int *dimension_matriz = readSharedInt(DIM_MATRIX);
    const int *num_restaurantes = readSharedInt(NUM_REST);
    char *matriz = readSharedMatrix();
    char espacio;
    int i, j;

    do
    {
        unsigned int seed = time(NULL);
        i = rand_r(&seed) % *dimension_matriz;
        j = rand_r(&seed) % *dimension_matriz;
        *num_r = rand_r(&seed) % *num_restaurantes;
        espacio = *(matriz + i * *dimension_matriz + j);
        if (espacio == ' ')
        {
            *(matriz + i * *dimension_matriz + j) = 'c';
            int x = j - (*dimension_matriz / 2);
            int y = (*dimension_matriz / 2) - i;
            Cliente c = {x, y, false, "", restaurantes[*num_r].pid, pid};
            enqueue(&restaurantes[*num_r].colaNumPipes, *counter);
            enqueue(&restaurantes[*num_r].colaClientes, pid);
            strcpy(c.pipePath, pipePath);
            clientes[*counter] = c;
            printClientInfo(&clientes[*counter], &restaurantes[*num_r]);
        }
    } while (espacio != ' ');
}