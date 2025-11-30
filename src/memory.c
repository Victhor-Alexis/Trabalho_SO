#include "../interfaces/memory.h"
#include <stdio.h>

/* ============================================================
   Inicialização da memória
   ============================================================ */

void init_memory(Memory *m)
{
    for (int i = 0; i < TOTAL_MEMORY; i++)
        m->blocks[i] = -1; // bloco livre
}

/* ============================================================
   Alocação para processos de tempo real (0–63)
   ============================================================ */

int allocate_realtime_memory(Memory *m, const Process *p)
{
    if (p->memory_blocks > REALTIME_MEMORY)
        return -1; // processo TR maior que a área TR permitida

    int free_count = 0;
    int start = 0;

    for (int i = 0; i < REALTIME_MEMORY; i++)
    {
        if (m->blocks[i] == -1)
        {
            // início de uma sequência livre
            if (free_count == 0)
                start = i;

            free_count++;

            // encontrou sequência contígua suficiente
            if (free_count == p->memory_blocks)
            {
                for (int j = start; j < start + p->memory_blocks; j++)
                    m->blocks[j] = p->pid;

                return start; // sucesso
            }
        }
        else
        {
            free_count = 0; // sequência quebrada
        }
    }

    return -1; // não há espaço contíguo
}

/* ============================================================
   Liberação de memória de tempo real
   ============================================================ */

void free_realtime_memory(Memory *m, const Process *p)
{
    for (int i = 0; i < REALTIME_MEMORY; i++)
    {
        if (m->blocks[i] == p->pid)
            m->blocks[i] = -1; // bloco liberado
    }
}
