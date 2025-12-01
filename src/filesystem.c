#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../interfaces/filesystem.h"
#include "../interfaces/process.h"

void fs_init_runtime(FileSystemRuntime *rt, FileSystemInput *input)
{
    rt->total_blocks = input->total_blocks;
    rt->file_count = 0;

    for (int i = 0; i < rt->total_blocks; i++)
        rt->blocks[i] = '0'; // vazio

    // carregar segmentos iniciais
    for (int i = 0; i < input->num_segments; i++)
    {
        FsFile *src = &input->segments[i];
        FsFile *dst = &rt->files[rt->file_count++];

        *dst = *src; // copy

        // marcar blocos no disco
        for (int b = 0; b < src->length; b++)
            rt->blocks[src->start_block + b] = src->name;
    }
}

int fs_find_free_space_first_fit(FileSystemRuntime *rt, int size)
{
    int start = 0;

    while (start + size <= rt->total_blocks)
    {
        int ok = 1;

        for (int i = 0; i < size; i++)
        {
            if (rt->blocks[start + i] != '0')
            {
                ok = 0;
                break;
            }
        }

        if (ok)
            return start;

        start++;
    }

    return -1;
}

int fs_create(FileSystemRuntime *rt, char name, int blocks, int pid, int op_index)
{
    // arquivo já existe?
    for (int i = 0; i < rt->file_count; i++)
        if (rt->files[i].name == name)
        {
            printf("Operacao %d => Falha\n", op_index + 1);
            printf("O arquivo %c ja existe.\n", name);
            return -1;
        }

    int start = fs_find_free_space_first_fit(rt, blocks);

    if (start < 0)
    {
        printf("Operacao %d => Falha\n", op_index + 1);
        printf("O processo %d nao pode criar o arquivo %c (falta de espaco).\n",
               pid, name);
        return -1;
    }

    // registrar
    FsFile *f = &rt->files[rt->file_count++];
    f->name = name;
    f->start_block = start;
    f->length = blocks;
    f->owner_pid = pid;

    // marcar blocos
    for (int i = 0; i < blocks; i++)
        rt->blocks[start + i] = name;

    printf("Operacao %d => Sucesso\n", op_index + 1);
    printf("O processo %d criou o arquivo %c (blocos %d ate %d).\n", pid, name, start, start + blocks - 1);

    return 0;
}

int fs_delete(FileSystemRuntime *rt, ProcessList *plist, char name, int pid, int op_index)
{
    // procurar arquivo
    FsFile *f = NULL;
    for (int i = 0; i < rt->file_count; i++)
        if (rt->files[i].name == name)
        {
            f = &rt->files[i];
            break;
        }

    if (!f)
    {
        printf("Operacao %d => Falha\n", op_index + 1);
        printf("O processo %d nao pode deletar o arquivo %c porque ele nao existe.\n",
               pid, name);
        return -1;
    }

    Process *proc = &plist->items[pid];
    int is_real_time = (proc->priority == 0);

    if (!(is_real_time || pid == f->owner_pid))
    {
        printf("Operacao %d => Falha\n", op_index + 1);
        printf("O processo %d nao pode deletar o arquivo %c (permissao negada).\n", pid, name);
        return -1;
    }

    // liberar blocos
    for (int i = 0; i < f->length; i++)
        rt->blocks[f->start_block + i] = '0';

    printf("Operacao %d => Sucesso\n", op_index + 1);
    printf("O processo %d deletou o arquivo %c.\n", pid, name);

    // invalida entrada
    f->name = '#';

    return 0;
}

void fs_print_map(FileSystemRuntime *rt)
{
    printf("Mapa de ocupacao do disco:\n");

    for (int i = 0; i < rt->total_blocks; i++)
    {
        printf("%c ", rt->blocks[i]);
    }
    printf("\n");
}

void fs_run(FileSystemInput *input, ProcessList *plist)
{
    FileSystemRuntime rt;
    fs_init_runtime(&rt, input);

    printf("\nSistema de arquivos =>\n");

    // Percorre todas as operações de sistema de arquivos lidas do arquivo files.txt
    for (int i = 0; i < input->num_operations; i++)
    {
        FsOperation *op = &input->operations[i];
        int pid = op->process_id;

        // processo existe?
        if (pid < 0 || pid >= (int)plist->count)
        {
            printf("Operacao %d => Falha\n", i + 1);
            printf("O processo %d nao existe.\n", pid);
            continue;
        }

        if (op->op_code == 0)
            fs_create(&rt, op->name, op->num_blocks, pid, i);
        else
            fs_delete(&rt, plist, op->name, pid, i);
    }

    fs_print_map(&rt);
}