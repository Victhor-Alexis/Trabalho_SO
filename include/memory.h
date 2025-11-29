#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

int alocar_memoria(int pid, int blocos, tipo_processo t);
void liberar_memoria(int pid);
void inicializar_memoria(void);

#endif /* MEMORY_H */
