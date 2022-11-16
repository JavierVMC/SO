#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define SMOBJ_NAME "/myMemoryObj"
#define DIM_MATRIX "/dimMatriz"

/* Lee la matriz de la memoria compartida */
char * readMatrix(){
    int fd;
    char *ptr;
    struct stat shmobj_st;
    fd = shm_open(SMOBJ_NAME, O_RDONLY, 00400);
    if(fd == -1){
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if(fstat(fd, &shmobj_st) == -1){
        printf("Error fstat\n");
        exit(1);
    }

    ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed in read process: %s\n", strerror(errno));
        exit(1);
    }
    return ptr;
}

/* Lee la dimension de la matriz de la memoria compartida */
const int * readDimensionOfMatrix(){
    int fd;
    const int *ptr;
    struct stat shmobj_st;
    fd = shm_open(DIM_MATRIX, O_RDONLY, 00400);
    if(fd == -1){
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if(fstat(fd, &shmobj_st) == -1){
        printf("Error fstat\n");
        exit(1);
    }

    ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed in read process: %s\n", strerror(errno));
        exit(1);
    }
    return ptr;
}

int main(){
    char * matriz = readMatrix();
    const int * dimension_matriz = readDimensionOfMatrix();
    
    int i, j;
    for (i = 0; i < *dimension_matriz; i++) {
        for (j = 0; j < *dimension_matriz; j++) {
        printf("%c ", *(matriz + i * *dimension_matriz + j));
        }
        printf("\n");
    }

    return 0;
}