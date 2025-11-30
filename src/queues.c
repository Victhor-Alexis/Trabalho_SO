#include <stdio.h>
#include <stdlib.h>
#include "../interfaces/queues.h"

/* ============================================================
   FUNÇÕES INTERNAS DE FILA
   ============================================================ */

void init_queue(ProcessQueue *q)
{
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

void init_queues(Queues *q)
{
    init_queue(&q->real_time_queue);

    for (int i = 0; i < NUM_USER_QUEUES; i++)
    {
        init_queue(&q->user_queues[i]);
    }
}

/*
 * Insere processo na fila (FIFO)
 */
int enqueue(ProcessQueue *queue, Process *p)
{
    if (queue->size >= MAX_QUEUE_SIZE)
        return 0; // fila cheia

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->items[queue->rear] = p;
    queue->size++;

    return 1;
}

/*
 * Remove processo
 */
Process *dequeue(ProcessQueue *queue)
{
    if (queue->size == 0)
        return NULL;

    Process *p = queue->items[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;

    return p;
}

/* ============================================================
   DISTRIBUIÇÃO DOS PROCESSOS NAS FILAS
   ============================================================ */

void distribute_processes_into_queues(const ProcessList *plist, Queues *q)
{
    init_queues(q);

    for (size_t i = 0; i < plist->count; i++)
    {
        Process *p = &plist->items[i];

        if (p->priority == 0)
        {
            // Fila de tempo real
            if (!enqueue(&q->real_time_queue, p))
                printf("[ERRO] Fila de tempo real cheia ao inserir PID %d\n", p->pid);
        }
        else if (p->priority >= 1 && p->priority <= 5)
        {
            int index = p->priority - 1; // prioridade 1 -> fila 0, etc.

            if (!enqueue(&q->user_queues[index], p))
                printf("[ERRO] Fila de usuário %d cheia ao inserir PID %d\n",
                       p->priority, p->pid);
        }
        else
        {
            printf("[AVISO] Processo PID %d com prioridade inválida (%d)\n",
                   p->pid, p->priority);
        }
    }
}

/* ============================================================
   DEBUG / LOG
   ============================================================ */

void print_queue(const ProcessQueue *q, const char *title)
{
    printf("\n%s (size=%d):\n", title, q->size);

    int idx = q->front;
    for (int i = 0; i < q->size; i++)
    {
        Process *p = q->items[idx];
        printf("  PID %d (prio %d)\n", p->pid, p->priority);
        idx = (idx + 1) % MAX_QUEUE_SIZE;
    }
}

void print_queues(const Queues *q)
{
    print_queue(&q->real_time_queue, "Fila Tempo Real (prio 0)");

    for (int i = 0; i < NUM_USER_QUEUES; i++)
    {
        char title[64];
        sprintf(title, "Fila Usuario (prio %d)", i + 1);
        print_queue(&q->user_queues[i], title);
    }
}
