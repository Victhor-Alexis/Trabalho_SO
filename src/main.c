#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "processes.h"
#include "filesys.h"
#include "utils.h"

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

    inicializar_basicas_globais();

    if (ler_processes(argv[1]) != 1)
    {
        printf("Erro ao ler processos\n");
        return 0;
    }

    if (ler_files(argv[2]) != 1)
    {
        printf("Erro ao ler arquivos\n");
        return 0;
    }

    executar_simulacao();
    return 0;
}
