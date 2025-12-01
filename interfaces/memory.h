#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"

#define TOTAL_MEMORY 1024
#define REALTIME_MEMORY 64 // memória reservada para processos de tempo real (0–63)
#define USER_START REALTIME_MEMORY
#define USER_MEMORY (TOTAL_MEMORY - REALTIME_MEMORY) // 960 blocos (64–1023)

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

/* Alocação/liberação para processos de tempo real (0–63) */
int allocate_realtime_memory(Memory *m, const Process *p);
void free_realtime_memory(Memory *m, const Process *p);

/* Alocação/liberação para processos de usuário (64–1023) */
int allocate_user_memory(Memory *m, const Process *p);
void free_user_memory(Memory *m, const Process *p);

#endif