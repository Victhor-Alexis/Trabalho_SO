#include <stdio.h>
#include "../interfaces/dispatcher.h"
#include "../interfaces/queues.h"
#include "../interfaces/memory.h"

void dispatch_process(const Process *p, int offset)
{
    if (!p)
        return;

    printf("\ndispatcher =>\n");
    printf("  PID: %d\n", p->pid);
    printf("  offset: %d\n", offset);
    printf("  blocks: %d\n", p->memory_blocks);
    printf("  priority: %d\n", p->priority);
    printf("  time: %d\n", p->cpu_time);
    printf("  scanners: %d\n", p->needs_scanner);
    printf("  printers: %d\n", p->printer_id);
    printf("  modems: %d\n", p->needs_modem);
    printf("  sata: %d\n", p->sata_disk_id);
}

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

// retorna o quantum associado à prioridade do processo de usuário
static int get_quantum_for_priority(int priority)
{
    // conforme especificação:
    // 1 -> 6, 2 -> 5, 3 -> 4, 4 -> 3, 5 -> 2
    switch (priority)
    {
    case 1:
        return 6;
    case 2:
        return 5;
    case 3:
        return 4;
    case 4:
        return 3;
    case 5:
        return 2;
    default:
        return 2; // fallback seguro
    }
}

/*
 * Retorna o índice da fila de usuário de maior prioridade que não está vazia.
 * (0 = prioridade 1, 1 = prioridade 2, ..., 4 = prioridade 5)
 * Retorna -1 se todas estiverem vazias.
 */
static int get_highest_nonempty_user_queue(const Queues *qs)
{
    for (int i = 0; i < NUM_USER_QUEUES; i++)
    {
        if (!queue_is_empty(&qs->user_queues[i]))
            return i;
    }
    return -1;
}

void run_dispatcher(Queues *qs, FileSystemInput *fs)
{
    (void)fs;

    Memory mem;
    init_memory(&mem);

    int current_time = 0; // tempo global
    int finished_processes = 0;

    /* ============================================================
       1. Executar processos de tempo real primeiro
       ============================================================ */
    while (!queue_is_empty(&qs->real_time_queue))
    {
        Process *p = dequeue(&qs->real_time_queue);

        if (p->start_time > current_time)
            current_time = p->start_time;

        int offset = allocate_realtime_memory(&mem, p);

        if (offset == -1)
        {
            enqueue(&qs->real_time_queue, p);
            continue;
        }

        printf("\n[ALLOCATION] PID %d allocated at offset %d", p->pid, offset);

        dispatch_process(p, offset);
        simulate_process_execution(p);

        free_realtime_memory(&mem, p);
        finished_processes++;
    }

    /* ============================================================
       2. Agora processos de usuário com preempção + aging
       ============================================================ */

    while (!all_user_queues_empty(qs))
    {
        int q_index = get_highest_nonempty_user_queue(qs);
        if (q_index < 0)
            break;

        Process *p = dequeue(&qs->user_queues[q_index]);

        /* Respeitar o tempo de start do processo */
        if (p->start_time > current_time)
            current_time = p->start_time;

        /* Por enquanto, sem memória do usuário */
        int offset = 64; // placeholder até implementarmos user memory

        /* Exibir dados do processo antes da execução */
        dispatch_process(p, offset);

        int quantum = get_quantum_for_priority(p->priority);

        if (p->cpu_time > quantum)
        {
            /* Processo não terminou -> preemptado */
            p->cpu_time -= quantum;

            int old = p->priority;

            if (p->priority < 5)
                p->priority++;

            int new_index = p->priority - 1;
            enqueue(&qs->user_queues[new_index], p);

            printf("[PREEMPT] PID %d did not finish. Priority %d -> %d, remaining=%d\n",
                   p->pid, old, p->priority, p->cpu_time);
        }
        else
        {
            /* Processo finaliza aqui */
            simulate_process_execution(p);
            finished_processes++;
        }

        /* Aging de processos esperando */
        apply_aging(qs);

        current_time++;
    }
}
