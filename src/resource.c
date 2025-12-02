#include "../interfaces/resource.h"
#include <stdio.h>

void init_resources(ResourceManager *rm)
{
    rm->scanner = -1;
    rm->modem = -1;

    for (int i = 0; i < 2; i++)
        rm->printer[i] = -1;

    for (int i = 0; i < 3; i++)
        rm->sata[i] = -1;
}

int allocate_resources(ResourceManager *rm, const Process *p)
{
    int requested_any = 0;

    /* ================= SCANNER ================= */
    if (p->needs_scanner == 1)
    {
        requested_any = 1;

        if (rm->scanner == -1)
        {
            rm->scanner = p->pid;
            printf("[RESOURCE] PID %d acquired SCANNER\n", p->pid);
        }
        else if (rm->scanner != p->pid)
        {
            return 0; // outro processo está usando → falha
        }
    }

    /* ================= PRINTER ================= */
    if (p->printer_id > 0)
    {
        requested_any = 1;

        int idx = p->printer_id - 1;

        if (rm->printer[idx] == -1)
        {
            rm->printer[idx] = p->pid;
            printf("[RESOURCE] PID %d acquired PRINTER %d\n", p->pid, p->printer_id);
        }
        else if (rm->printer[idx] != p->pid)
        {
            return 0; // falha → ocupada por outro processo
        }
    }

    /* ================= MODEM ================= */
    if (p->needs_modem == 1)
    {
        requested_any = 1;

        if (rm->modem == -1)
        {
            rm->modem = p->pid;
            printf("[RESOURCE] PID %d acquired MODEM\n", p->pid);
        }
        else if (rm->modem != p->pid)
        {
            return 0;
        }
    }

    /* ================= SATA ================= */
    if (p->sata_disk_id > 0)
    {
        requested_any = 1;

        int idx = p->sata_disk_id - 1;

        if (rm->sata[idx] == -1)
        {
            rm->sata[idx] = p->pid;
            printf("[RESOURCE] PID %d acquired SATA %d\n", p->pid, p->sata_disk_id);
        }
        else if (rm->sata[idx] != p->pid)
        {
            return 0;
        }
    }

    if (!requested_any)
        return 2; // não precisa de recursos

    return 1; // sucesso
}

void free_resources(ResourceManager *rm, const Process *p)
{
    if (p->needs_scanner == 1)
    {
        rm->scanner = 1;
        printf("[RESOURCE] PID %d released SCANNER\n", p->pid);
    }

    if (p->printer_id > 0)
    {
        int idx = p->printer_id - 1;
        rm->printer[idx] = 1;
        printf("[RESOURCE] PID %d released PRINTER %d\n", p->pid, p->printer_id);
    }

    if (p->needs_modem == 1)
    {
        rm->modem = 1;
        printf("[RESOURCE] PID %d released MODEM\n", p->pid);
    }

    if (p->sata_disk_id > 0)
    {
        int idx = p->sata_disk_id - 1;
        rm->sata[idx] = 1;
        printf("[RESOURCE] PID %d released SATA %d\n", p->pid, p->sata_disk_id);
    }
}
