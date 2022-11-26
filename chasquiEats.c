#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE_OF_SMOBJ 8000000
#define MATRIX_NAME "/matriz"

#define DIM_MATRIX "/dimMatriz"
#define NUM_REST "/numRestaurantes"
#define NUM_MOT "/numMotorizados"
#define INTERVALO_MS "/intervaloMs"

#define REST_NAME "/restaurantes"
#define MOT_NAME "/motorizados"

#define MAX_NUM_REST 100
#define MAX_NUM_MOT 100
#define MAX_DIM_MATRIX 63

typedef struct restaurante
{
    int x;
    int y;
    int number;
}Restaurante;

typedef struct motorizado
{
    int x;
    int y;
    int number;
}Motorizado;

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

/* Creacion de memoria compartida para almacenar el arreglo de motorizados */
Motorizado *createSharedMotorizados()
{
    int fd;
    Motorizado *ptr;
    fd = shm_open(MOT_NAME, O_CREAT | O_RDWR, 00700);
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

int main(int argc, char *argv[])
{
    int opt, index;

    while ((opt = getopt(argc, argv, "h")) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_help(argv[0]);
            return 0;
        case '?':
        default:
            fprintf(stderr, "uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", argv[0]);
            fprintf(stderr, "     %s -h\n", argv[0]);
            return 1;
        }
    }

    if (argc != 6)
    {
        fprintf(stderr, "uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", argv[0]);
        fprintf(stderr, "     %s -h\n", argv[0]);
        return 1;
    }

    const int dimension_matriz = atoi(argv[1]);
    const int num_restaurantes = atoi(argv[2]);
    const int intervalo_ms = atoi(argv[3]);
    const int num_motorizados = atoi(argv[4]);
    const int distancia_km = atoi(argv[5]);

    if (dimension_matriz % 2 == 0 
        || num_motorizados > MAX_NUM_MOT 
        || num_restaurantes > MAX_NUM_REST 
        || dimension_matriz > MAX_DIM_MATRIX 
        || num_restaurantes == 0 
        || num_motorizados == 0 
        || intervalo_ms == 0 
        || distancia_km == 0)
    {
        fprintf(stderr, "uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", argv[0]);
        fprintf(stderr, "Restricciones:\n");
        fprintf(stderr, "<dimersion_matriz>Numero entero positivo impar menor o igual a 63.\n");
        fprintf(stderr, "<restaurantes>\tNumero entero positivo menor o igual a 100.\n");
        fprintf(stderr, "<intervalo_ms>\tIntervalo de tiempo en ms.\n");
        fprintf(stderr, "<motorizados>\tNumero entero positivo menor o igual a 100.\n");
        fprintf(stderr, "<distancia_km>\tDistancia maxima a la que el motorizado debe estar del restaurante para ser elegido.\n");
        fprintf(stderr, "     %s -h\n", argv[0]);
        return 1;
    }

    printf("Grilla de %dx%d, %d restaurantes, intervalo %d ms, %d motorizados, %d kilometros de distancia\n", dimension_matriz, dimension_matriz, num_restaurantes, intervalo_ms, num_motorizados, distancia_km);

    char matriz[dimension_matriz][dimension_matriz];
    int i = 0;
    int j = 0;

    memset(matriz, ' ', sizeof(char) * dimension_matriz * dimension_matriz);

    /* Muestra la matriz en consola */
    for (i = 0; i < dimension_matriz; i++)
    {
        for (j = 0; j < dimension_matriz; j++)
        {
            printf("%c ", matriz[i][j]);
        }
        printf("\n");
    }

    /* Guarda la matriz en memoria compartida */
    char *ptr = createSharedMatrix();
    memcpy(ptr, matriz, sizeof(matriz));


    /* Guarda la cantidad de elementos de las estructuras (matriz, restaurantes, motorizados) en memoria compartida*/
    createSharedInt(dimension_matriz, DIM_MATRIX);
    createSharedInt(num_restaurantes, NUM_REST);
    createSharedInt(num_motorizados, NUM_MOT);
    createSharedInt(intervalo_ms, INTERVALO_MS);

    return 0;
}
