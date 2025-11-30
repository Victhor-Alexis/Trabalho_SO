#include <stdio.h>
#include "../interfaces/dispatcher.h"
#include "../interfaces/queues.h"

void simulate_process_execution(Process *p, int run_time)
{
    if (!p || run_time <= 0)
        return;

    int start_inst = p->cpu_time - p->remaining_cpu_time + 1;
    int end_inst   = start_inst + run_time - 1;

    // Se é a primeira execução:
    if (start_inst == 1)
        printf("\nP%d STARTED\n", p->pid);
    else
        printf("\nP%d RESUMED\n", p->pid);

    for (int i = start_inst; i <= end_inst; i++)
    {
        printf("P%d instruction %d\n", p->pid, i);
    }

    p->remaining_cpu_time -= run_time;

    if (p->remaining_cpu_time == 0)
        printf("P%d return SIGINT\n", p->pid);
    else
        printf("P%d INTERRUPTED\n", p->pid);
}

void run_dispatcher(Queues *qs, FileSystemInput *fs)
{
    (void)fs;

    /*
     * 1. Prioridade 0 (tempo real) — SEM QUANTUM
     */
    while (!queue_is_empty(&qs->real_time_queue))
    {
        Process *p = dequeue(&qs->real_time_queue);

        if (p->remaining_cpu_time == 0)
            p->remaining_cpu_time = p->cpu_time;

        simulate_process_execution(p, p->remaining_cpu_time);
    }

    /*
     * 2. Filas de usuário (1..5) com quantum + realimentação
     */
    for (int i = 0; i < NUM_USER_QUEUES; i++)
    {
        ProcessQueue *current_queue = &qs->user_queues[i];

        while (!queue_is_empty(current_queue))
        {
            Process *p = dequeue(current_queue);

            if (p->remaining_cpu_time == 0)
                p->remaining_cpu_time = p->cpu_time;

            int quantum = get_quantum(p->priority);

            int run_time = p->remaining_cpu_time > quantum
                ? quantum
                : p->remaining_cpu_time;

            simulate_process_execution(p, run_time);

            if (p->remaining_cpu_time > 0)
            {
                // processo preemptado -> aumenta prioridade para favorecer novos processos
                if (p->priority > 5)
                    p->priority++;

                // reinsere na fila correta
                int new_index = p->priority - 1;
                enqueue(&qs->user_queues[new_index], p);
            }
        }
    }
}


