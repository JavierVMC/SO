#include <stdbool.h>
#include <pthread.h>
#include "queue.h"

#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

#define SIZE_OF_SMOBJ 8000000
#define MATRIX_NAME "/matriz"

#define DIM_MATRIX "/dimMatriz"
#define NUM_REST "/numRestaurantes"
#define NUM_MOT "/numMotorizados"
#define NUM_CLT "/numClientes"
#define INTERVALO_MS "/intervaloMs"

#define REST_NAME "/restaurantes"
#define MOT_NAME "/motorizados"
#define CLT_NAME "/clientes"

#define MAX_NUM_REST 100
#define MAX_NUM_MOT 100
#define MAX_DIM_MATRIX 63
#define MAX_CLIENTS 10
#define LEN_MESSAGE 100
#define MAX_DIGITS 10


typedef struct cliente
{
    int x;
    int y;
    bool served;
    char pipePath[LEN_MESSAGE];
    pthread_t pid_r;
    pthread_t pid;
} Cliente;

typedef struct restaurante
{
    int x;
    int y;
    pthread_t pid;
    Queue colaClientes;
    Queue colaNumPipes;
} Restaurante;

typedef struct motorizado
{
    int x;
    int y;
    pthread_t pid;
} Motorizado;


void print_help(char *command);

char *createSharedMatrix();
char *readSharedMatrix();
void printSharedMatrix();

void createSharedInt(const int num, const char *name);
const int *readSharedInt(const char *name);

Restaurante *createSharedRestaurants();
Restaurante *readSharedRestaurants();
Restaurante *editSharedRestaurants();

int msleep(long msec);

void printSharedMatrix();
void printSharedRestaurants();
void printSharedClients();

void createPipePath(char *pipePath, int i);

void printQueue(Queue *queue);
void printQueueArray(Queue *queue);
void printClientInfo(Cliente *cliente, Restaurante *restaurante);
void printRestaurantInfo(Restaurante *restaurante);

void createRestaurant(pthread_t pid, int *num_r, int *counter, Restaurante *restaurantes);
void createClient(pthread_t pid, int *num_r, int *counter, char *pipePath, Restaurante *restaurantes, Cliente *clientes);

#endif
