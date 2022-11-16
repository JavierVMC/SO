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

#define SIZE_OF_SMOBJ 100000
#define SMOBJ_NAME "/myMemoryObj"

#define SIZE_OF_DIM_MATRIZ 4
#define DIM_MATRIX "/dimMatriz"

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


void print_help(char *command)
{
    printf("chasquiEats inicializa la simulacion del servicio de pedidos.\n");
    printf("uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

/* Creacion de memoria compartida para almacenar la matriz */
char * createSharedMatrix(){
    int fd;
    char *ptr;
    fd = shm_open(SMOBJ_NAME, O_CREAT | O_RDWR, 00700);
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

void createSharedDimMatrix(const int dimension_matriz){
    int fd;
    int *ptr;
    fd = shm_open(DIM_MATRIX, O_CREAT | O_RDWR, 00700);
    if (fd == -1)
    {
        printf("Error file descriptor\n");
        exit(1);
    }
    if (ftruncate(fd, sizeof(SIZE_OF_DIM_MATRIZ)) == -1)
    {
        printf("Error shared memory cannot be resized\n");
    }
    ptr = mmap(NULL, sizeof(SIZE_OF_DIM_MATRIZ), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    memcpy(ptr, &dimension_matriz, sizeof(int));
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

    printf("Grilla de %dx%d, %d restaurantes, intervalo %d ms, %d motorizados, %d kilometros de distancia\n", dimension_matriz, dimension_matriz, num_restaurantes, intervalo_ms, num_motorizados, distancia_km);

    char matriz[dimension_matriz][dimension_matriz];
    int i = 0;
    int j = 0;

    memset(matriz, ' ', sizeof(char) * dimension_matriz * dimension_matriz);

    srand(time(0));

    struct restaurante restaurantes[num_restaurantes];
    struct motorizado motorizados[num_motorizados];

    char espacio;
    int num_r = 0;
    int num_m = 0;

    /*Distribucion aleatoria de restaurantes*/
    for (num_r; num_r < num_restaurantes; num_r++)
    {
        do
        {
            i = rand() % dimension_matriz;
            j = rand() % dimension_matriz;
            espacio = matriz[i][j];
            if (espacio == ' ')
            {
                matriz[i][j] = 'r';
                struct restaurante r = {j - (dimension_matriz / 2), (dimension_matriz / 2) - i, num_r};
                restaurantes[num_r] = r;
            }
        } while (espacio != ' ');
    }

    /*Distribucion aleatoria de motorizados*/
    for (num_m; num_m < num_motorizados; num_m++)
    {
        do
        {
            i = rand() % dimension_matriz;
            j = rand() % dimension_matriz;
            espacio = matriz[i][j];
            if (espacio == ' ')
            {
                matriz[i][j] = 'm';
                struct motorizado m = {j - (dimension_matriz / 2), (dimension_matriz / 2) - i, num_m};
                motorizados[num_m] = m;
            }
        } while (espacio != ' ');
    }

    /* Muestra la matriz en consola */
    i = 0;
    j = 0;
    for (i; i < dimension_matriz; i++)
    {
        j = 0;
        for (j; j < dimension_matriz; j++)
        {
            printf("%c ", matriz[i][j]);
        }
        printf("\n");
    }

    /* Muestra los restarurantes y su ubicacion */
    num_r = 0;

    for (num_r; num_r < num_restaurantes; num_r++)
    {
        printf("Restaurante %d en (%d,%d)\n", restaurantes[num_r].number, restaurantes[num_r].x, restaurantes[num_r].y);
    }

    /* Muestra los motorizados y su ubicacion */
    num_m = 0;

    for (num_m; num_m < num_motorizados; num_m++)
    {
        printf("Motorizado %d en (%d,%d)\n", motorizados[num_m].number, motorizados[num_m].x, motorizados[num_m].y);
    }

    char * ptr = createSharedMatrix();
    memcpy(ptr, matriz, sizeof(matriz));
    createSharedDimMatrix(dimension_matriz);

    return 0;
}
