#ifndef QUEUES_H
#define QUEUES_H

/* fila simples para as filas do escalonador */
void inicializar_filas(void);
void fila_push(int id, int pid);
int fila_pop(int id);
int fila_vazia(int id);

#endif /* QUEUES_H */
