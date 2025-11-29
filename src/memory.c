#include "memory.h"
#include "globals.h"
#include <string.h>

void inicializar_memoria(void)
{
    for (int i = 0; i < MEM_TOTAL; ++i)
        mapa_memoria[i] = -1;
}

/* Alocacao de memoria continua - first-fit dentro da regiao apropriada */
int alocar_memoria(int pid, int blocos, tipo_processo t)
{
    int inicio = (t == TEMPO_REAL) ? 0 : MEM_RT;
    int limite = (t == TEMPO_REAL) ? MEM_RT : MEM_TOTAL;
    int cont = 0;
    for (int i = inicio; i < limite; ++i)
    {
        if (mapa_memoria[i] == -1)
        {
            cont++;
            if (cont == blocos)
            {
                int primeira = i - blocos + 1;
                for (int j = primeira; j <= i; ++j)
                    mapa_memoria[j] = pid;
                return primeira;
            }
        }
        else
            cont = 0;
    }
    return -1;
}

void liberar_memoria(int pid)
{
    for (int i = 0; i < MEM_TOTAL; i++)
        if (mapa_memoria[i] == pid)
            mapa_memoria[i] = -1;
}
