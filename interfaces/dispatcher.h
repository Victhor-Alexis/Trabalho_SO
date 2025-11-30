#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "process.h"
#include "filesystem.h"
#include "queues.h"

/*
 * Estrutura simples para carregar o estado do dispatcher.
 * Depois podemos expandir com ponteiro para memória, recursos, etc.
 */
typedef struct
{
    int current_time;    // no futuro podemos simular tempo
    int next_mem_offset; // alocação sequencial simples por enquanto
} DispatcherState;

/*
 * Função principal: recebe a lista de processos e o sistema de arquivos
 * e simula (por enquanto) a criação/execução básica.
 */
void run_dispatcher(Queues *qs, FileSystemInput *fs);

/* Apenas para esta fase: simula a execução do processo */
void simulate_process_execution(const Process *p);

#endif
