#include "lexer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* concat(char* str, char c)
{
    size_t len = 0;
    if(str)
        len = strlen(str);

    char *str2 = (char*)malloc(sizeof(char)*(len + 1 + 1) );

    if(str)
        strcpy(str2, str);


    str2[len] = c;
    str2[len + 1] = '\0';

    if(str)
        free(str);

    return str2;
}

struct protocol_lexer_t
{
    char                    previous;
    char                    current;
    char*                   source;
    unsigned                iterator;
    unsigned long           source_length;
    protocol_token_t*       current_token;
};


void protocol_lexer_t_create(protocol_lexer_t** lexer, const char* source, unsigned long length)
{
    (*lexer)                    = (protocol_lexer_t*)malloc(sizeof(protocol_lexer_t)*1);
    (*lexer)->source            = (char*)malloc(sizeof(char)*length);
    (*lexer)->source_length     = length;
    (*lexer)->iterator          = 0;
    (*lexer)->current           = source[(*lexer)->iterator];
    (*lexer)->previous          = '\0';
    (*lexer)->current_token     =  NULL;

    strcpy((*lexer)->source, source);

    (*lexer)->current_token     = protocol_lexer_t_get_next_token((*lexer));
}

protocol_token_t* protocol_lexer_t_get_current_token(protocol_lexer_t* lexer)
{
    return lexer->current_token;
}

void protocol_lexer_t_destroy(struct protocol_lexer_t** lexer)
{
    if((*lexer)->current_token)
        protocol_token_t_destroy(&(*lexer)->current_token);

    free((*lexer)->source);
    free((*lexer));

    (*lexer) = NULL;
}

void protocol_lexer_t_advance(protocol_lexer_t* lexer)
{
    lexer->previous = lexer->current;
    lexer->current = lexer->source[++lexer->iterator];
}

int protocol_lexer_t_is_eof(protocol_lexer_t* lexer)
{
    return lexer->iterator == lexer->source_length + 1;
}


protocol_token_t* protocol_lexer_t_get_next_token(protocol_lexer_t* lexer)
{
    // if(lexer->current_token) protocol_token_t_destroy(&lexer->current_token);

    protocol_token_t* token = NULL;

    if(protocol_lexer_t_is_eof(lexer) || lexer->current == '\n')
    {
        protocol_token_t_create(&token, "\0", PROTOCOL_TOKEN_EOF);
        lexer->current_token = token;
        return token;
    }

    while(lexer->current == ' ')
        protocol_lexer_t_advance(lexer);


    if(lexer->current == '+' && lexer->previous == '\0')
    {
        protocol_token_t_create(&token, "+", PROTOCOL_TOKEN_PLUS);
        protocol_lexer_t_advance(lexer);
        lexer->current_token = token;
        return token;
    }
    else if(lexer->current == '-' && lexer->previous == '\0')
    {
        protocol_token_t_create(&token, "-", PROTOCOL_TOKEN_MINUS);
        protocol_lexer_t_advance(lexer);
        lexer->current_token = token;
        return token;
    }
    else if(lexer->current == '#' && lexer->previous == ' ')
    {
        protocol_token_t_create(&token, " #", PROTOCOL_TOKEN_TAG);
        protocol_lexer_t_advance(lexer);
        lexer->current_token = token;
        return token;
    }

    char* result = NULL;
    protocol_token_type_t type = PROTOCOL_TOKEN_KEYWORD;

    while(
        ! protocol_lexer_t_is_eof(lexer)
        && (lexer->current != ' ' && lexer->current != '\0' && lexer->current != '\n')
    )
    {
        result = concat(result, lexer->current);
        protocol_lexer_t_advance(lexer);
    }

    protocol_token_t_create(&token, result, type);

    lexer->current_token = token;
    return token;
}
