#include <stdio.h>
#include "../interfaces/dispatcher.h"
#include "../interfaces/queues.h"

void simulate_process_execution(const Process *p)
{
    if (!p)
        return;

    printf("\nprocess %d =>\n", p->pid);
    printf("P%d STARTED\n", p->pid);

    for (int i = 1; i <= p->cpu_time; i++)
    {
        printf("P%d instruction %d\n", p->pid, i);
    }

    printf("P%d return SIGINT\n", p->pid);
}

void run_dispatcher(Queues *qs, FileSystemInput *fs)
{
    (void)fs;

    // 1. processar primeiro a fila de tempo real
    while (!queue_is_empty(&qs->real_time_queue))
    {
        Process *p = dequeue(&qs->real_time_queue);
        simulate_process_execution(p);
    }

    // 2. depois processar as 5 filas de usuário
    for (int i = 0; i < NUM_USER_QUEUES; i++)
    {
        while (!queue_is_empty(&qs->user_queues[i]))
        {
            Process *p = dequeue(&qs->user_queues[i]);
            simulate_process_execution(p);
        }
    }
}
