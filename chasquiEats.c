#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_help(char *command)
{
    printf("chasquiEats inicializa la simulacion del servicio de pedidos.\n");
    printf("uso:\n %s <dimension_matriz> <restaurantes> <intervalo_ms> <motorizados> <distancia_km>\n", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tAyuda, muestra este mensaje\n");
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

    int dimension_matriz = atoi(argv[1]);
    int restaurantes = atoi(argv[2]);
    int intervalo_ms = atoi(argv[3]);
    int motorizados = atoi(argv[4]);
    int distancia_km = atoi(argv[5]);

    printf("%d\n", dimension_matriz);
    printf("%d\n", restaurantes);
    printf("%d\n", intervalo_ms);
    printf("%d\n", motorizados);
    printf("%d\n", distancia_km);

    return 0;
}
