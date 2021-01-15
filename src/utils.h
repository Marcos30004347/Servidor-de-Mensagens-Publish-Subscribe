#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#ifdef DEBUG 
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...) 
#endif

#endif
