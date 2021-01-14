#include "hash.h"

#define INT32_MAX 2147483647

int hash_string(const char *s)
{
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31*hashval;
    return hashval % INT32_MAX;
}
