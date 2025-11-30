#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"

#define TOTAL_MEMORY 1024
#define REALTIME_MEMORY 64 // memória reservada para processos de tempo real (0–63)
#define USER_MEMORY 960    // memória para processos de usuário (64–1023)

/*
 * Estrutura principal da memória:
 * Cada posição representa um bloco.
 * -1  = livre
 * pid = bloco pertencente ao processo com aquele PID
 */
typedef struct
{
    int blocks[TOTAL_MEMORY];
} Memory;

/* Inicializa toda a memória como livre */
void init_memory(Memory *m);

/* Aloca memória para processo de tempo real (retorna offset ou -1) */
int allocate_realtime_memory(Memory *m, const Process *p);

/* Libera memória ocupada por um processo de tempo real */
void free_realtime_memory(Memory *m, const Process *p);

#endif
