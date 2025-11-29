#ifndef PROCESSES_H
#define PROCESSES_H

#include "types.h"

int ler_processes(const char *nome_arquivo);
void imprimir_despacho(processo *p);
int quantum_da_prioridade(int prioridade);

#endif /* PROCESSES_H */
