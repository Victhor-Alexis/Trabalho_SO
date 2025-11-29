#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"

/* Variaveis globais definidas em src/global.c (mas aqui apenas declaradas) */
extern processo proc[MAX_PROCS];
extern int num_processos;

extern int mapa_memoria[MEM_TOTAL];
extern int discos_total;
extern arquivo_disco arquivos[MAX_DISK_FILES];

extern int impressoras_ocupadas[MAX_PRINTERS];
extern int scanner_ocupado;
extern int modem_ocupado;
extern int sata_ocupado[MAX_SATA];

extern int filas[MAX_QUEUES][MAX_PROCS];
extern int f_inicio[MAX_QUEUES];
extern int f_fim[MAX_QUEUES];

extern fsop ops_fs[1000];
extern int num_ops_fs;

#endif /* GLOBALS_H */
