#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>

/* Representa um processo lido do arquivo processes.txt */
typedef struct
{
    int pid;                   // atribuído na leitura: 0, 1, 2, ...
    int start_time;            // tempo de inicialização
    int priority;              // prioridade inicial
    int cpu_time;              // tempo de processador
    int executed_instructions; // quantas instruções já executou
    int memory_blocks;         // blocos de memória requeridos
    int in_memory;             // 0 = ainda não alocado, 1 = já em memória
    int mem_offset;            // início do bloco de memória alocado
    int printer_id;            // número-código da impressora requisitada
    int needs_scanner;         // 0 ou 1
    int needs_modem;           // 0 ou 1
    int sata_disk_id;          // número-código do disco
} Process;

typedef struct
{
    Process *items;
    size_t count;
} ProcessList;

/* Simula a execução do processo real time */
void simulate_process_execution(const Process *p);

/* Simula a execução do processo de usuario */
void simulate_process_slice(Process *p, int quantum);

#endif /* PROCESS_H */
