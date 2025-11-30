#ifndef UTILS_H
#define UTILS_H

#include "process.h"
#include "filesystem.h"

/*
 * Funções de leitura dos arquivos de entrada
 */
ProcessList *load_processes_from_file(const char *path);
FileSystemInput *load_filesystem_from_file(const char *path);

/*
 * Funções de log
 */
void print_process(const Process *p);
void print_process_list(const ProcessList *list);

void print_fs_file(const FsFile *f);
void print_fs_operation(const FsOperation *op);
void print_filesystem_input(const FileSystemInput *fs);

/*
 * Destruidores
 */
void destroy_process_list(ProcessList *list);
void destroy_filesystem_input(FileSystemInput *fs);

#endif
