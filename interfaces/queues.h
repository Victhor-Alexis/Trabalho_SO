#ifndef QUEUES_H
#define QUEUES_H

#include "process.h"

#define MAX_QUEUE_SIZE 100
#define NUM_USER_QUEUES 5
#define REAL_TIME_QUEUE 0

/*
 * Estrutura interna de uma única fila de processos
 */
typedef struct
{
    Process *items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} ProcessQueue;

/*
 * Estrutura global contendo:
 * - 1 fila de tempo real
 * - 5 filas de usuário (prioridades 1 a 5)
 */
typedef struct
{
    ProcessQueue real_time_queue;              // prioridade 0
    ProcessQueue user_queues[NUM_USER_QUEUES]; // prioridades 1..5
} Queues;

/* Inicializa todas as filas */
void init_queues(Queues *q);

/* Insere processo na fila apropriada */
int enqueue(ProcessQueue *queue, Process *p);

/* Remove processo (não vamos implementar agora, só deixar pronto) */
Process *dequeue(ProcessQueue *queue);

/*
 * Distribui os processos da ProcessList nas filas:
 * - prioridade 0 → fila de tempo real
 * - prioridade 1..5 → filas de usuário correspondentes
 */
void distribute_processes_into_queues(const ProcessList *plist, Queues *q);

/* Funções de debug */
void print_queues(const Queues *q);

void log_aging(const Process *p, int old_priority);

int queue_is_empty(const ProcessQueue *q);

int get_quantum_for_priority(int priority);

int get_highest_nonempty_user_queue(const Queues *qs);

/* Retorna 1 se todas as filas de usuário estiverem vazias */
int all_user_queues_empty(const Queues *q);

/* Aplica aging: promove alguns processos das filas de menor prioridade */
void apply_aging(Queues *q);

#endif
