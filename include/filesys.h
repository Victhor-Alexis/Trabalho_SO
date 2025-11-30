#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"

int criar_arquivo(char *nome, int blocos, int dono);
int deletar_arquivo(char *nome, int pid, int is_rt);
int ler_files(const char *nome_arquivo);
void inicializa_arquivos_no_disco(void);
void preenche_informacoes_arquivo(char *nome, int inicio, int blocos);

#endif /* FILESYS_H */
