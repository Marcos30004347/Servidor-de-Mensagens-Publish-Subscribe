#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG 
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...) 
#endif

int verify_char(const char c)
{
    if(
           (c >= 48 && c <= 57) // numbers
        || (c >= 65 && c <= 90) // upper characters
        || (c >= 97 && c <= 122) // lower characters
        || c == ' '
        || c == ','
        || c == '.'
        || c == '?'
        || c == '!'
        || c == ':'
        || c == ';'
        || c == '+'
        || c == '-'
        || c == '*'
        || c == '/'
        || c == '='
        || c == '@'
        || c == '#'
        || c == '$'
        || c == '%'
        || c == '('
        || c == ')'
        || c == '['
        || c == ']'
        || c == '{'
        || c == '}'
        || c == '\n'
    ) return 1;

    return 0;
}

int parse_message(const char* message, unsigned long length, char*** tags, unsigned long* n_tags)
{
    char** parsed_tags = NULL;
    unsigned long n = 0;

    char back = '\0';
    
    char* tag = NULL;

    int tag_state = 0;

    for(int i=0; i<length; i++)
    {
        if(i>0)
            back = message[i-1];

        printf("processing %c with back %c and state %i at %i\n", message[i], back, tag_state, i);

        if(tag_state) {
            printf("C\n");    
            if(message[i] == ' ' || message[i] == '\n') {
                tag_state = 0;

                char** tmp = (char**)malloc(sizeof(char*)*++n);

                for(int j=0; j < n - 1; j++)
                {
                    tmp[j] = parsed_tags[j];
                }
                
                if(parsed_tags)
                    free(parsed_tags);
        
                parsed_tags = tmp;
                printf("copying %s\n", tag);
                // parsed_tags[n - 1] = malloc(sizeof(char)*tag_l);
                parsed_tags[n - 1] = (char*)malloc(sizeof(char)*strlen(tag));
                strcpy(parsed_tags[n - 1], tag);
                tag = NULL;
                printf("asdasd\n");
                continue;
            }
            else if(message[i] == '#') {

                tag_state = 0;
                tag = NULL;
                continue;
            }
            printf("J\n");    

            char c = message[i];

            size_t len = 0;
            if(tag)
                len = strlen(tag);

            char *str2 = (char*)malloc(sizeof(char)*(len + 1 + 1) );
    
            if(tag)
                strcpy(str2, tag);
    
    
            str2[len] = message[i];
            str2[len + 1] = '\0';

            if(tag)
                free(tag);

            tag = str2;

            printf("tag = %s\n", tag);
            continue;
        } 
        else if(message[i] == '#' && back == ' ')
        {
            tag_state = 1;
        }
    
    }
    printf("aqui carai\n");
    printf("asdsdasds ds %s\n", parsed_tags[0]);
    (*tags) = parsed_tags;
    (*n_tags) = n;

    return 0;
}

#endif
