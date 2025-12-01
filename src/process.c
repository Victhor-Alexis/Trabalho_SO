#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../interfaces/process.h"

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

// imprime a execução parcial de um processo de usuário (quantum)
void simulate_process_slice(Process *p, int quantum)
{
    if (!p)
        return;

    printf("\nprocess %d =>\n", p->pid);

    if (p->executed_instructions == 0)
        printf("P%d STARTED\n", p->pid);
    else
        printf("P%d CONTINUED\n", p->pid);

    int instructions_to_run = quantum;

    if (p->cpu_time <= quantum)
        instructions_to_run = p->cpu_time;

    // execute instructions
    for (int i = 0; i < instructions_to_run; i++)
    {
        int instr_num = p->executed_instructions + 1;
        printf("P%d instruction %d\n", p->pid, instr_num);
        p->executed_instructions++;
    }

    // if finished
    if (p->cpu_time <= quantum)
    {
        printf("P%d return SIGINT\n", p->pid);
        p->cpu_time = 0; // <--- THIS FIXES THE INFINITE LOOP
    }
}
