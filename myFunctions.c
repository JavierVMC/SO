#include "myFunctions.h"
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
    return ptr;
}

/* Lee el arreglo de restaurantes de la memoria compartida */
Restaurante *editSharedRestaurants()
{
    int fd;
    Restaurante *ptr;
    struct stat shmobj_st;
    fd = shm_open(REST_NAME, O_RDWR, 00700);
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
        printf("Restaurante [%ld] (%d,%d) {%s} -> Cliente [%ld]\n", ptr_read[i].pid, ptr_read[i].x, ptr_read[i].y, ptr_read[i].pipePath, ptr_read[i].pid_c);
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