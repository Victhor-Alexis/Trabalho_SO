#include <stdio.h>
#include "../interfaces/dispatcher.h"
#include "../interfaces/queues.h"
#include "../interfaces/memory.h"
#include "../interfaces/process.h"

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

void run_dispatcher(Queues *qs)
{
    Memory mem;
    init_memory(&mem);

    int current_time = 0; // tempo global
    int finished_processes = 0;

    /* Processos tempo real */
    while (!queue_is_empty(&qs->real_time_queue))
    {
        // chama uma funcao que desinfilera todos que estiverem "prontos"
        Process *p = dequeue(&qs->real_time_queue);

        int offset = allocate_realtime_memory(&mem, p);

        if (offset == -1)
        {
            // mata esse processo porque nunca vair rodar
            enqueue(&qs->real_time_queue, p);
            continue;
        }

        printf("\n[ALLOCATION] PID %d allocated at offset %d", p->pid, offset);

        dispatch_process(p, offset);
        simulate_process_execution(p);

        free_realtime_memory(&mem, p);
        finished_processes++;
    }

    /* Processos usuario */
    int aging_counter = 0;
    int AGING_INTERVAL = 3;

    while (!all_user_queues_empty(qs))
    {
        int q_index = get_highest_nonempty_user_queue(qs);
        if (q_index < 0)
            break;

        Process *p = dequeue(&qs->user_queues[q_index]);

        if (!p->in_memory)
        {
            int offset = allocate_user_memory(&mem, p);

            if (offset == -1)
            {
                printf("\n[USER MEM] PID %d cannot be allocated (no space). Waiting...\n", p->pid);
                enqueue(&qs->user_queues[q_index], p);

                // Aging também ocorre aqui
                aging_counter++;
                if (aging_counter >= AGING_INTERVAL)
                {
                    apply_aging(qs);
                    aging_counter = 0;
                }

                current_time++;
                continue;
            }

            p->in_memory = 1;
            p->mem_offset = offset;
            printf("\n[ALLOCATION USER] PID %d allocated at offset %d\n", p->pid, offset);
        }

        /* Exibir dados do processo antes da execução */
        dispatch_process(p, p->mem_offset);
        int quantum = get_quantum_for_priority(p->priority);

        if (p->cpu_time > quantum)
        {
            /* Executa somente um slice */
            simulate_process_slice(p, quantum);

            /* Reduz tempo restante */
            p->cpu_time -= quantum;

            /* Rebaixa prioridade */
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
            /* Executa apenas as instruções restantes e retorna SIGINT */
            simulate_process_slice(p, quantum);

            /* Processo terminou: liberar memória de usuário */
            free_user_memory(&mem, p);
            p->in_memory = 0;
            p->mem_offset = -1;

            finished_processes++;
        }

        /* Aging de processos esperando */
        aging_counter++;
        if (aging_counter >= AGING_INTERVAL)
        {
            apply_aging(qs);
            aging_counter = 0; // resetar o contador
        }

        current_time++;
    }
}
