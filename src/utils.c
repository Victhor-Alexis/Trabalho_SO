#include "utils.h"
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
