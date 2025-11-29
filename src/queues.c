#include "queues.h"
#include "globals.h" /* declara variaveis globais (filas, indices) */
#include <stddef.h>

void inicializar_filas(void)
{
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        f_inicio[i] = f_fim[i] = 0;
    }
}

void fila_push(int id, int pid)
{
    if (id < 0 || id >= MAX_QUEUES)
        return;
    filas[id][f_fim[id]++] = pid;
}

int fila_pop(int id)
{
    if (id < 0 || id >= MAX_QUEUES)
        return -1;
    if (f_inicio[id] >= f_fim[id])
        return -1;
    return filas[id][f_inicio[id]++];
}

int fila_vazia(int id)
{
    if (id < 0 || id >= MAX_QUEUES)
        return 1;
    return (f_inicio[id] >= f_fim[id]);
}
