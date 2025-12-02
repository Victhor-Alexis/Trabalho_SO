#ifndef RESOURCE_H
#define RESOURCE_H

#include "process.h"

typedef struct
{
    int scanner;    // -1 = livre, PID se ocupado
    int printer[2]; // 0 ou 1
    int modem;      // -1 livre
    int sata[3];    // 0,1,2

} ResourceManager;

void init_resources(ResourceManager *rm);

int allocate_resources(ResourceManager *rm, const Process *p);
void free_resources(ResourceManager *rm, const Process *p);

#endif
