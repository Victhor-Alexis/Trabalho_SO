#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../interfaces/utils.h"

/* ============================================================
   CONFIGURAÇÕES
   ============================================================ */

#define MAX_PROCESSES 600

/* ============================================================
   FUNÇÕES DE PROCESSOS
   ============================================================ */

ProcessList *load_processes_from_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        fprintf(stderr, "[ERRO] Não foi possível abrir %s\n", path);
        return NULL;
    }

    Process *array = malloc(MAX_PROCESSES * sizeof(Process));
    if (!array)
    {
        fclose(fp);
        fprintf(stderr, "[ERRO] Falha ao alocar lista de processos.\n");
        return NULL;
    }

    size_t count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        int st, pr, cpu, mem, prn, scn, mod, sata;

        int parsed = sscanf(
            line,
            " %d , %d , %d , %d , %d , %d , %d , %d",
            &st, &pr, &cpu, &mem, &prn, &scn, &mod, &sata);

        if (parsed != 8)
            continue;

        if (count >= MAX_PROCESSES)
        {
            fprintf(stderr,
                    "[ERRO] O arquivo tem mais de %d processos.\n",
                    MAX_PROCESSES);
            break;
        }

        Process *p = &array[count];
        p->pid = (int)count;
        p->start_time = st;
        p->priority = pr;
        p->cpu_time = cpu;
        p->executed_instructions = 0;
        p->memory_blocks = mem;
        p->in_memory = 0;
        p->mem_offset = -1;
        p->printer_id = prn;
        p->needs_scanner = scn;
        p->needs_modem = mod;
        p->sata_disk_id = sata;

        count++;
    }

    fclose(fp);

    ProcessList *plist = malloc(sizeof(ProcessList));
    plist->items = array;
    plist->count = count;

    return plist;
}

void destroy_process_list(ProcessList *list)
{
    if (!list)
        return;
    free(list->items);
    free(list);
}

void print_process(const Process *p)
{
    if (!p)
        return;

    printf("P%d: start=%d priority=%d cpu=%d mem=%d prn=%d scn=%d mod=%d sata=%d\n",
           p->pid, p->start_time, p->priority, p->cpu_time,
           p->memory_blocks, p->printer_id,
           p->needs_scanner, p->needs_modem, p->sata_disk_id);
}

void print_process_list(const ProcessList *list)
{
    printf("\n--- Processos (%zu) ---\n", list->count);

    for (size_t i = 0; i < list->count; i++)
        print_process(&list->items[i]);
}

/* Ler files.txt */

FileSystemInput *load_filesystem_from_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        fprintf(stderr, "[ERRO] Não foi possível abrir %s\n", path);
        return NULL;
    }

    FileSystemInput *fs = malloc(sizeof(FileSystemInput));
    if (!fs)
    {
        fclose(fp);
        return NULL;
    }

    memset(fs, 0, sizeof(FileSystemInput));

    char line[256];
    int line_count = 0;

    // -------- Linha 1: total de blocos --------
    if (!fgets(line, sizeof(line), fp))
    {
        fprintf(stderr, "[ERRO] Arquivo vazio.\n");
        free(fs);
        fclose(fp);
        return NULL;
    }
    sscanf(line, " %d", &fs->total_blocks);
    line_count++;

    // -------- Linha 2: número de segmentos --------
    if (!fgets(line, sizeof(line), fp))
    {
        fprintf(stderr, "[ERRO] Arquivo incompleto.\n");
        free(fs);
        fclose(fp);
        return NULL;
    }
    sscanf(line, " %d", &fs->num_segments);
    line_count++;

    if (fs->num_segments > MAX_FS_SEGMENTS)
    {
        fprintf(stderr,
                "[ERRO] Segmentos (%d) excedem máximo (%d).\n",
                fs->num_segments, MAX_FS_SEGMENTS);
        free(fs);
        fclose(fp);
        return NULL;
    }

    // -------- Ler segmentos --------
    for (int i = 0; i < fs->num_segments; i++)
    {
        if (!fgets(line, sizeof(line), fp))
        {
            fprintf(stderr, "[ERRO] EOF inesperado nos segmentos.\n");
            free(fs);
            fclose(fp);
            return NULL;
        }
        line_count++;

        char name;
        int start, len;

        sscanf(line, " %c , %d , %d", &name, &start, &len);

        fs->segments[i].name = name;
        fs->segments[i].start_block = start;
        fs->segments[i].length = len;
        fs->segments[i].owner_pid = -1;
    }

    // -------- Ler operações --------
    fs->num_operations = 0;

    while (fgets(line, sizeof(line), fp))
    {
        line_count++;

        if (line_count > MAX_FS_LINES)
        {
            fprintf(stderr,
                    "[ERRO] O arquivo excede %d linhas.\n",
                    MAX_FS_LINES);
            break;
        }

        if (fs->num_operations >= MAX_FS_OPERATIONS)
        {
            fprintf(stderr,
                    "[ERRO] Excedeu máximo de %d operações.\n",
                    MAX_FS_OPERATIONS);
            break;
        }

        int pid, code, blocks = 0;
        char name;

        int parsed = sscanf(line, " %d , %d , %c , %d",
                            &pid, &code, &name, &blocks);

        FsOperation *op = &fs->operations[fs->num_operations];

        if (parsed == 4)
        {
            op->process_id = pid;
            op->op_code = code;
            op->name = name;
            op->num_blocks = blocks;
        }
        else if (parsed == 3)
        {
            op->process_id = pid;
            op->op_code = code;
            op->name = name;
            op->num_blocks = 0;
        }
        else
        {
            continue;
        }

        fs->num_operations++;
    }

    fclose(fp);
    return fs;
}

void destroy_filesystem_input(FileSystemInput *fs)
{
    free(fs);
}

void print_fs_file(const FsFile *f)
{
    printf("Arquivo %c: start=%d len=%d owner=%d\n",
           f->name, f->start_block, f->length, f->owner_pid);
}

void print_fs_operation(const FsOperation *op)
{
    if (!op)
        return;

    if (op->op_code == 0)
        printf("CREATE: processo %d, arquivo %c, blocos %d\n",
               op->process_id, op->name, op->num_blocks);
    else
        printf("DELETE: processo %d, arquivo %c\n",
               op->process_id, op->name);
}

void print_filesystem_input(const FileSystemInput *fs)
{
    printf("\n=== Sistema de Arquivos ===\n");
    printf("Total de blocos: %d\n", fs->total_blocks);
    printf("Segmentos iniciais: %d\n\n", fs->num_segments);

    for (int i = 0; i < fs->num_segments; i++)
        print_fs_file(&fs->segments[i]);

    printf("\nOperacoes: %d\n", fs->num_operations);

    for (int i = 0; i < fs->num_operations; i++)
        print_fs_operation(&fs->operations[i]);
}
