#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>

#define MAX_FS_SEGMENTS 100   // máx. segmentos existentes
#define MAX_FS_OPERATIONS 900 // máx. operações de arquivo
#define MAX_FS_LINES 1000     // limite total do arquivo

typedef struct
{
    char name;       // letra do arquivo
    int start_block; // início do segmento
    int length;      // quantidade de blocos ocupados
    int owner_pid;   // -1 se não associado
} FsFile;

typedef struct
{
    int process_id; // processo responsável
    int op_code;    // 0=create, 1=delete
    char name;      // nome do arquivo
    int num_blocks; // usado apenas para create
} FsOperation;

typedef struct
{
    int total_blocks; // linha 1
    int num_segments; // linha 2

    FsFile segments[MAX_FS_SEGMENTS]; // FIXO!

    int num_operations;
    FsOperation operations[MAX_FS_OPERATIONS]; // FIXO!
} FileSystemInput;

#endif
