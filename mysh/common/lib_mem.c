
#include <string.h> //memcpy,memset

int sh_strlen(char *str)
{
    int i = 0;
    while(*str++ != '\0')
        i++;
    return i;
}

int sh_memset(void *p, int val, int size)
{
    memset(p, val, size);
    return 0;
}

int sh_memcpy(void *dst, void *src, int size)
{
    memcpy(dst, src, size);
    return 0;
}
