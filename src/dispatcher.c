#include <stdio.h>
#include "../interfaces/dispatcher.h"
#include "../interfaces/queues.h"
#include "../interfaces/memory.h"

void dispatch_process(const Process *p, int offset)
{
    if (!p)
        return;

    // saída igual ao exemplo da especificação
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

void run_dispatcher(Queues *qs, FileSystemInput *fs)
{
    (void)fs;

    Memory mem;
    init_memory(&mem); // inicializar memória

    // 1. processar primeiro a fila de tempo real
    while (!queue_is_empty(&qs->real_time_queue))
    {
        Process *p = dequeue(&qs->real_time_queue);
        int offset = allocate_realtime_memory(&mem, p);

        if (offset == -1)
        {
            // não há espaço → processo volta para fila TR
            enqueue(&qs->real_time_queue, p);
            continue;
        }

        // Printa a alocacao
        printf("\n[ALLOCATION] PID %d allocated at offset %d", p->pid, offset);

        // imprime o bloco do dispatcher ANTES da execução
        dispatch_process(p, offset);

        // Executa processo
        simulate_process_execution(p);

        // libera memória após execução
        free_realtime_memory(&mem, p);
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
