#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void print_help(char *command)
{
    printf("chasquiEats inicializa la simulacion del servicio de pedidos.\n");
    printf("uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

struct restaurante
{
    int x;
    int y;
    char name[30];
};

struct motorizado
{
    int x;
    int y;
    char name[30];
};

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

    int dimension_matriz = atoi(argv[1]);
    int num_restaurantes = atoi(argv[2]);
    int intervalo_ms = atoi(argv[3]);
    int num_motorizados = atoi(argv[4]);
    int distancia_km = atoi(argv[5]);

    printf("Grilla de %dx%d, %d restaurantes, intervalo %d ms, %d motorizados, %d kilometros de distancia\n", dimension_matriz, dimension_matriz, num_restaurantes, intervalo_ms, num_motorizados, distancia_km);

    char matriz[dimension_matriz][dimension_matriz];
    int i = 0;
    int j = 0;

    memset(matriz, ' ', sizeof(char) * dimension_matriz * dimension_matriz);

    srand(time(0));

    struct restaurante restaurantes[num_restaurantes];
    struct motorizado motorizados[num_motorizados];

    char espacio;

    /*Distribucion aleatoria de restaurantes*/
    for (num_restaurantes; num_restaurantes > 0; num_restaurantes--)
    {
        do
        {
            i = rand() % dimension_matriz;
            j = rand() % dimension_matriz;
            espacio = matriz[i][j];
            if (espacio == ' ')
            {
                matriz[i][j] = 'r';
            }
        } while (espacio != ' ');
    }

    /*Distribucion aleatoria de motorizados*/
    for (num_motorizados; num_motorizados > 0; num_motorizados--)
    {
        do
        {
            i = rand() % dimension_matriz;
            j = rand() % dimension_matriz;
            espacio = matriz[i][j];
            if (espacio == ' ')
            {
                matriz[i][j] = 'm';
            }
        } while (espacio != ' ');
    }

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

    return 0;
}
