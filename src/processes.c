#include "processes.h"
#include "globals.h"
#include "utils.h"
#include <stdio.h>

int quantum_da_prioridade(int prioridade)
{
    switch (prioridade)
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
        return 2;
    }
}

void imprimir_despacho(processo *p)
{
    printf("dispatcher =>\n");
    printf("    PID: %d\n", p->pid);
    printf("    offset: %d\n", p->offset_memoria);
    printf("    blocks: %d\n", p->blocos_memoria);
    printf("    priority: %d\n", p->prioridade);
    printf("    time: %d\n", p->tempo_cpu);
    printf("    scanners: %d\n", p->req_scanner);
    printf("    printers: %d\n", p->req_impressora);
    printf("    modems: %d\n", p->req_modem);
    printf("    sata: %d\n\n", p->req_sata);
}

/* Leitura do arquivo processes.txt */
int ler_processes(const char *nome_arquivo)
{
    FILE *f = fopen(nome_arquivo, "r");
    if (!f)
    {
        perror("Erro ao abrir processes.txt");
        return 0;
    }

    char linha[MAX_LINE];
    int pid = 0;

    while (fgets(linha, sizeof(linha), f))
    {
        char *s = trim(linha);
        if (*s == '\0' || *s == '#')
            continue;

        int t_init, prioridade, tproc, blocos, impressora, scanner, modem, sata;
        int lidos = sscanf(s, " %d , %d , %d , %d , %d , %d , %d , %d", &t_init, &prioridade, &tproc, &blocos, &impressora, &scanner, &modem, &sata);

        if (lidos < 8)
            continue;
        if (pid >= MAX_PROCS)
            break;

        processo *p = &processos_prontos[pid];
        p->pid = pid;
        p->tempo_inicio = t_init;
        p->prioridade = prioridade;
        p->tempo_cpu = tproc;
        p->blocos_memoria = blocos;
        p->req_impressora = impressora;
        p->req_scanner = scanner;
        p->req_modem = modem;
        p->req_sata = sata;
        p->offset_memoria = -1;
        p->restante = tproc;
        p->finalizado = false;
        p->aging = 0;
        p->tipo = (prioridade == 0 ? TEMPO_REAL : USUARIO);
        pid++;
    }

    num_processos = pid;
    fclose(f);
    return 1;
}
