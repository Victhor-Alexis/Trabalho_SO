#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_PROCS 100
#define MAX_QUEUES 6
#define MEM_TOTAL 1024
#define MEM_RT 64
#define DISK_MAX_BLOCKS 10000
#define MAX_FILENAME 32
#define MAX_DISK_FILES 1000
#define MAX_LINE 256
#define MAX_PRINTERS 2
#define MAX_SATA 3

typedef enum
{
    TEMPO_REAL = 0,
    USUARIO = 1
} tipo_processo;

typedef struct
{
    int pid;
    tipo_processo tipo;
    int tempo_inicio;
    int prioridade; /* 0 para realtime, 1..5 para usuario */
    int tempo_cpu;
    int blocos_memoria;
    int req_impressora; /* 0 = nenhuma, 1..MAX_PRINTERS */
    int req_scanner;    /* 0 ou 1 */
    int req_modem;      /* 0 ou 1 */
    int req_sata;       /* 0 = nenhum, 1..MAX_SATA */
    int offset_memoria;
    int restante;
    bool finalizado;
    int aging;
} processo;

typedef struct
{
    bool usado;
    char nome[MAX_FILENAME];
    int inicio;
    int blocos;
    int dono;
} arquivo_disco;

/* Operacao de sistema de arquivos */
typedef struct
{
    int pid;
    int codigo; /* 0=create, 1=delete */
    char nome[MAX_FILENAME];
    int blocos;
    int executada;
} fsop;

#endif /* TYPES_H */
