#ifndef UTILS_H
#define UTILS_H

#ifdef DEBUG 
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

#endif
