#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "processes.h"
#include "filesys.h"

/*
    Rode make no diretório do projeto.
    Execute ./dispatcher processes.txt files.txt.
*/

void executar_simulacao(void);

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Uso: %s processes.txt files.txt\n", argv[0]);
        return 1;
    }
    /* inicializacoes basicas de globais */
    for (int i = 0; i < MEM_TOTAL; i++)
        mapa_memoria[i] = -1;
    for (int i = 0; i < MAX_DISK_FILES; i++)
        arquivos[i].usado = false;

    if (!ler_processes(argv[1]))
        return 1;
    if (!ler_files(argv[2]))
        return 1;

    executar_simulacao();
    return 0;
}
