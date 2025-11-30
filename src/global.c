/* definicoes das variaveis globais */
#include "../include/types.h"

processo processos_prontos[MAX_PROCS];
int num_processos = 0;

int mapa_memoria[MEM_TOTAL];
int discos_total = 0;
arquivo_disco arquivos[MAX_DISK_FILES];

int impressoras_ocupadas[MAX_PRINTERS];
int scanner_ocupado = 0;
int modem_ocupado = 0;
int sata_ocupado[MAX_SATA];

int filas[MAX_QUEUES][MAX_PROCS];
int f_inicio[MAX_QUEUES], f_fim[MAX_QUEUES];

fsop ops_fs[1000];
int num_ops_fs = 0;
