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
#include "myFunctions.h"

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
