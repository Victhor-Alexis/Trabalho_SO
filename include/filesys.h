#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"

int criar_arquivo(char *nome, int blocos, int dono);
int deletar_arquivo(char *nome, int pid, int is_rt);
int ler_files(const char *nome_arquivo);

#endif /* FILESYS_H */
