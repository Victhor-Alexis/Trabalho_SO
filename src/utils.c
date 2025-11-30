#include "utils.h"
#include "globals.h"
#include <string.h>

char *trim(char *s)
{
    if (!s)
        return s;
    while (*s == ' ' || *s == '\t')
        s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t'))
    {
        *end = '\0';
        end--;
    }
    return s;
}

void inicializar_basicas_globais()
{
    // Inicializar memoria total
    for (int i = 0; i < MEM_TOTAL; i++)
        mapa_memoria[i] = -1;

    // Inicializar Disco
    for (int i = 0; i < MAX_DISK_FILES; i++)
        arquivos[i].usado = false;
}
