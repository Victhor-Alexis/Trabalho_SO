#include "processes.h"
#include "queues.h"
#include "memory.h"
#include "filesys.h"
#include "globals.h"
#include <stdio.h>

/* Corpo da simulação — preserva lógica original */
void executar_simulacao(void)
{
    /* inicializacoes */
    inicializar_memoria();
    for (int i = 0; i < MAX_PRINTERS; i++)
        impressoras_ocupadas[i] = -1;
    for (int i = 0; i < MAX_SATA; i++)
        sata_ocupado[i] = 0;
    scanner_ocupado = 0;
    modem_ocupado = 0;
    inicializar_filas();

    for (int i = 0; i < num_processos; i++)
    {
        processo *p = &proc[i];
        int off = alocar_memoria(p->pid, p->blocos_memoria, p->tipo);
        p->offset_memoria = off;
        imprimir_despacho(p);
        if (off == -1)
        {
            printf("Process %d => FALHA: memoria insuficiente (%d blocos)\n\n", p->pid, p->blocos_memoria);
            p->finalizado = true;
            continue;
        }
        if (p->tipo == TEMPO_REAL)
            fila_push(0, p->pid);
        else
        {
            int pr = p->prioridade;
            if (pr < 1)
                pr = 5;
            if (pr > 5)
                pr = 5;
            fila_push(pr, p->pid);
        }
    }

    int ativos = 0;
    for (int i = 0; i < num_processos; i++)
        if (!proc[i].finalizado)
            ativos++;

    int tempo_global = 0;

    while (ativos > 0)
    {
        int pid_executar = -1;
        int qid = -1;
        if (!fila_vazia(0))
        {
            pid_executar = fila_pop(0);
            qid = 0;
        }
        else
        {
            for (int pr = 1; pr <= 5; pr++)
            {
                if (!fila_vazia(pr))
                {
                    pid_executar = fila_pop(pr);
                    qid = pr;
                    break;
                }
            }
        }
        if (pid_executar == -1)
        {
            tempo_global++;
            for (int i = 0; i < num_processos; i++)
            {
                if (!proc[i].finalizado)
                    proc[i].aging++;
                if (!proc[i].finalizado && proc[i].tipo == USUARIO && proc[i].aging >= 10)
                {
                    if (proc[i].prioridade > 1)
                    {
                        proc[i].prioridade -= 1;
                        proc[i].aging = 0;
                        fila_push(proc[i].prioridade, proc[i].pid);
                    }
                }
            }
            continue;
        }

        processo *atual = &proc[pid_executar];
        if (atual->finalizado)
        {
            ativos--;
            continue;
        }

        /* verifica recursos */
        int recursos_ok = 1;
        if (atual->req_impressora > 0)
        {
            int idx = atual->req_impressora - 1;
            if (idx < 0 || idx >= MAX_PRINTERS)
                recursos_ok = 0;
            else if (impressoras_ocupadas[idx] != -1 && impressoras_ocupadas[idx] != atual->pid)
                recursos_ok = 0;
        }
        if (atual->req_scanner)
        {
            if (scanner_ocupado && !scanner_ocupado)
                recursos_ok = 0;
        }
        if (atual->req_modem)
        {
            if (modem_ocupado && !modem_ocupado)
                recursos_ok = 0;
        }
        if (atual->req_sata > 0)
        {
            int sidx = atual->req_sata - 1;
            if (sidx < 0 || sidx >= MAX_SATA)
                recursos_ok = 0;
            else if (sata_ocupado[sidx])
                recursos_ok = 0;
        }

        if (!recursos_ok)
        {
            printf("process %d =>\nP%d BLOCKED (recursos)\n\n", atual->pid, atual->pid);
            if (atual->tipo == TEMPO_REAL)
                fila_push(0, atual->pid);
            else
                fila_push(atual->prioridade, atual->pid);
            for (int i = 0; i < num_processos; i++)
                if (!proc[i].finalizado && proc[i].pid != atual->pid)
                    proc[i].aging++;
            tempo_global++;
            continue;
        }

        /* aloca recursos */
        if (atual->req_impressora > 0)
            impressoras_ocupadas[atual->req_impressora - 1] = atual->pid;
        if (atual->req_scanner)
            scanner_ocupado = 1;
        if (atual->req_modem)
            modem_ocupado = 1;
        if (atual->req_sata > 0)
            sata_ocupado[atual->req_sata - 1] = 1;

        printf("process %d =>\n", atual->pid);
        printf("P%d STARTED\n", atual->pid);

        int quantum = 0;
        if (qid == 0)
            quantum = atual->restante;
        else
            quantum = quantum_da_prioridade(atual->prioridade);
        if (quantum > atual->restante)
            quantum = atual->restante;

        for (int t = 1; t <= quantum; t++)
        {
            printf("P%d instruction %d\n", atual->pid, t);
            tempo_global++;
        }

        atual->restante -= quantum;

        if (atual->restante <= 0)
        {
            printf("P%d return SIGINT\n\n", atual->pid);
            atual->finalizado = true;
            ativos--;
            if (atual->req_impressora > 0)
                impressoras_ocupadas[atual->req_impressora - 1] = -1;
            if (atual->req_scanner)
                scanner_ocupado = 0;
            if (atual->req_modem)
                modem_ocupado = 0;
            if (atual->req_sata > 0)
                sata_ocupado[atual->req_sata - 1] = 0;
            liberar_memoria(atual->pid);
        }
        else
        {
            if (atual->tipo == USUARIO)
            {
                if (atual->prioridade < 5)
                    atual->prioridade += 1;
                fila_push(atual->prioridade, atual->pid);
            }
            else
                fila_push(0, atual->pid);
            if (atual->req_impressora > 0)
                impressoras_ocupadas[atual->req_impressora - 1] = -1;
            if (atual->req_scanner)
                scanner_ocupado = 0;
            if (atual->req_modem)
                modem_ocupado = 0;
            if (atual->req_sata > 0)
                sata_ocupado[atual->req_sata - 1] = 0;
            printf("P%d NOT FINISHED, requeued with priority %d\n\n", atual->pid, atual->prioridade);
        }

        /* executa operacoes de arquivo pendentes para este PID */
        for (int i = 0; i < num_ops_fs; i++)
        {
            if (ops_fs[i].executada)
                continue;
            if (ops_fs[i].pid != atual->pid)
                continue;
            printf("Operacao (PID %d) => ", ops_fs[i].pid);
            if (ops_fs[i].codigo == 0)
            {
                int res = criar_arquivo(ops_fs[i].nome, ops_fs[i].blocos, atual->pid);
                if (res == 0)
                {
                    /* encontra o arquivo recem-criado para imprimir inicio..fim */
                    int inicio = -1, blocos = ops_fs[i].blocos;
                    for (int f = 0; f < MAX_DISK_FILES; f++)
                    {
                        if (arquivos[f].usado && strcmp(arquivos[f].nome, ops_fs[i].nome) == 0)
                        {
                            inicio = arquivos[f].inicio;
                            break;
                        }
                    }
                    if (inicio >= 0)
                    {
                        printf("Sucesso\nO processo %d criou o arquivo %s (blocos %d..%d).\n\n", atual->pid, ops_fs[i].nome, inicio, inicio + blocos - 1);
                    }
                    else
                    {
                        printf("Sucesso\nO processo %d criou o arquivo %s (blocos ?).\n\n", atual->pid, ops_fs[i].nome);
                    }
                }
                else
                {
                    printf("Falha\nO processo %d nao pode criar o arquivo %s (falta de espaco).\n\n", atual->pid, ops_fs[i].nome);
                }
                ops_fs[i].executada = 1;
            }
            else if (ops_fs[i].codigo == 1)
            {
                int res = deletar_arquivo(ops_fs[i].nome, atual->pid, atual->tipo == TEMPO_REAL);
                if (res == 0)
                {
                    printf("Sucesso\nO processo %d deletou o arquivo %s.\n\n", atual->pid, ops_fs[i].nome);
                }
                else if (res == 1)
                {
                    printf("Falha\nO processo %d nao pode deletar o arquivo %s porque ele nao existe.\n\n", atual->pid, ops_fs[i].nome);
                }
                else
                {
                    printf("Falha\nO processo %d nao pode deletar o arquivo %s (sem permissao).\n\n", atual->pid, ops_fs[i].nome);
                }
                ops_fs[i].executada = 1;
            }
            else
            {
                printf("Operacao desconhecida\n\n");
                ops_fs[i].executada = 1;
            }
        }

        /* aging: incrementa espera e promove se necessario */
        for (int j = 0; j < num_processos; j++)
        {
            if (!proc[j].finalizado && proc[j].pid != atual->pid)
            {
                proc[j].aging++;
                if (proc[j].tipo == USUARIO && proc[j].aging >= 20)
                {
                    if (proc[j].prioridade > 1)
                    {
                        proc[j].prioridade -= 1;
                        proc[j].aging = 0;
                        fila_push(proc[j].prioridade, proc[j].pid);
                    }
                }
            }
        }
    } /* end while */

    /* imprimir mapa do disco */
    printf("Mapa de ocupacao do disco:\n");
    char *map = malloc(discos_total + 1);
    if (!map)
        return;
    for (int i = 0; i < discos_total; i++)
        map[i] = '0';
    for (int f = 0; f < MAX_DISK_FILES; f++)
    {
        if (arquivos[f].usado)
        {
            char letra = arquivos[f].nome[0] ? arquivos[f].nome[0] : 'F';
            for (int b = 0; b < arquivos[f].blocos; b++)
            {
                int pos = arquivos[f].inicio + b;
                if (pos >= 0 && pos < discos_total)
                    map[pos] = letra;
            }
        }
    }
    for (int i = 0; i < discos_total; i++)
        printf("%c ", map[i]);
    printf("\n");
    free(map);
}
