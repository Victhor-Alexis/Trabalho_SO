#include <stdio.h>
#include "../interfaces/utils.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Uso: %s processes.txt files.txt\n", argv[0]);
        return 1;
    }

    ProcessList *plist = load_processes_from_file(argv[1]);
    FileSystemInput *fs = load_filesystem_from_file(argv[2]);

    print_process_list(plist);
    print_filesystem_input(fs);

    destroy_process_list(plist);
    destroy_filesystem_input(fs);

    return 0;
}
