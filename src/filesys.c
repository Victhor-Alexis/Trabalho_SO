#include "filesys.h"
#include "globals.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inicializa_arquivos_no_disco(void)
{
    for (int i = 0; i < MAX_DISK_FILES; i++)
        arquivos[i].usado = false;
}

/* Sistema de arquivos: alocacao contigua first-fit */
int criar_arquivo(char *nome, int blocos, int dono)
{
    int inicio = -1, cont = 0;
    for (int i = 0; i < discos_total; i++)
    {
        int ocupado = 0;
        for (int f = 0; f < MAX_DISK_FILES; f++)
        {
            if (arquivos[f].usado)
            {
                int s = arquivos[f].inicio;
                int e = arquivos[f].inicio + arquivos[f].blocos - 1;
                if (i >= s && i <= e)
                {
                    ocupado = 1;
                    break;
                }
            }
        }
        if (!ocupado)
        {
            cont++;
            if (cont == blocos)
            {
                inicio = i - blocos + 1;
                break;
            }
        }
        else
            cont = 0;
    }
    if (inicio == -1)
        return -1;
    for (int f = 0; f < MAX_DISK_FILES; f++)
    {
        if (!arquivos[f].usado)
        {
            arquivos[f].usado = true;
            strncpy(arquivos[f].nome, nome, MAX_FILENAME - 1);
            arquivos[f].nome[MAX_FILENAME - 1] = '\0';
            arquivos[f].inicio = inicio;
            arquivos[f].blocos = blocos;
            arquivos[f].dono = dono;
            return 0;
        }
    }
    return -1;
}

int deletar_arquivo(char *nome, int pid, int is_rt)
{
    for (int f = 0; f < MAX_DISK_FILES; f++)
    {
        if (arquivos[f].usado && strcmp(arquivos[f].nome, nome) == 0)
        {
            if (is_rt || arquivos[f].dono == pid)
            {
                arquivos[f].usado = false;
                return 0;
            }
            return 2; /* sem permissao */
        }
    }
    return 1; /* nao encontrado */
}

void preenche_informacoes_arquivo(char *nome, int inicio, int blocos)
{
    // First fit
    for (int i = 0; i < MAX_DISK_FILES; i++)
    {
        if (!arquivos[i].usado)
        {
            arquivos[i].usado = true;
            strncpy(arquivos[i].nome, trim(nome), 1);
            arquivos[i].nome[MAX_FILENAME - 1] = '\0';
            arquivos[i].inicio = inicio;
            arquivos[i].blocos = blocos;
            arquivos[i].dono = -1;
            break;
        }
    }
}

/* Leitura do arquivo files.txt */
int ler_files(const char *nome_arquivo)
{
    inicializa_arquivos_no_disco();

    FILE *f = fopen(nome_arquivo, "r");
    if (!f)
    {
        perror("Erro ao abrir files.txt");
        return 0;
    }

    // Le a primeira linha para definir a quantidade de blocos do disco
    char linha[MAX_LINE];
    if (!fgets(linha, sizeof(linha), f))
    {
        fclose(f);
        return 0;
    }

    discos_total = atoi(trim(linha));

    // duvida: Ta certo isso? "DISK_MAX_BLOCKS" nao deveria ser o proprio "discos_total"
    if (discos_total <= 0 || discos_total > DISK_MAX_BLOCKS)
    {
        fprintf(stderr, "Numero invalido de blocos no disco: %d\n", discos_total);
        fclose(f);
        return 0;
    }

    // Le a segunda linha para definir a quantidade de blocos usados no disco
    if (!fgets(linha, sizeof(linha), f))
    {
        fclose(f);
        return 0;
    }
    int segmentos_ocupados = atoi(trim(linha));

    // Le as proximas n (segmentos ocupados) linhas
    // Para cada segmento ocupado, le as informacoes do aqruivo presente no segmento
    for (int i = 0; i < segmentos_ocupados; i++)
    {
        if (!fgets(linha, sizeof(linha), f))
            break;
        char nome_do_arquivo[1];
        int inicio, blocos;
        if (sscanf(linha, " %[^,] , %d , %d", nome_do_arquivo, &inicio, &blocos) == 3)
        {
            preenche_informacoes_arquivo(nome_do_arquivo, inicio, blocos);
        }
        else
        {
            printf("FALHA: parametros insuficientes para o aqrquivo %s", nome_do_arquivo);
        }
    }

    // Le o restante das linhas, que representam opercoes a serem realizadas pelo sistema de arquivos
    num_ops_fs = 0;
    while (fgets(linha, sizeof(linha), f))
    {
        char *s = trim(linha);
        if (*s == '\0')
            continue;
        int pidop, code;
        char nomeop[MAX_FILENAME];
        int blocosop = 0;
        int lidos = sscanf(s, " %d , %d , %[^,] , %d", &pidop, &code, nomeop, &blocosop);
        if (lidos >= 3)
        {
            ops_fs[num_ops_fs].pid = pidop;
            ops_fs[num_ops_fs].codigo = code;
            strncpy(ops_fs[num_ops_fs].nome, trim(nomeop), MAX_FILENAME - 1);
            ops_fs[num_ops_fs].nome[MAX_FILENAME - 1] = '\0';
            ops_fs[num_ops_fs].blocos = (lidos == 4 ? blocosop : 0);
            ops_fs[num_ops_fs].executada = 0;
            num_ops_fs++;
            if (num_ops_fs >= 1000)
                break;
        }
    }
    fclose(f);
    return 1;
}
