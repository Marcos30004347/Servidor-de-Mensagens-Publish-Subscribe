#include "lexer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


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
    ) return 0;

    return -1;
}

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


int protocol_lexer_t_create(protocol_lexer_t** lexer, const char* source, unsigned long length)
{
    (*lexer)                    = (protocol_lexer_t*)malloc(sizeof(protocol_lexer_t)*1);
    (*lexer)->source            = (char*)malloc(sizeof(char)*length);
    (*lexer)->source_length     = length;
    (*lexer)->iterator          = 0;
    (*lexer)->current           = source[(*lexer)->iterator];
    (*lexer)->previous          = '\0';
    (*lexer)->current_token     =  NULL;

    strcpy((*lexer)->source, source);

    return protocol_lexer_t_get_next_token((*lexer), &(*lexer)->current_token);
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

int protocol_lexer_t_advance(protocol_lexer_t* lexer)
{
    lexer->previous = lexer->current;
    lexer->current = lexer->source[++lexer->iterator];
    return verify_char(lexer->current);
}

int protocol_lexer_t_is_eof(protocol_lexer_t* lexer)
{
    return lexer->iterator == lexer->source_length + 1;
}

char* copy(const char* token)
{
    char* r = (char*)malloc(sizeof(char)*strlen(token));
    strcpy(r, token);
    return r;
}


int protocol_lexer_t_get_next_token(protocol_lexer_t* lexer, protocol_token_t** token)
{

    if(lexer->current_token)
        protocol_token_t_destroy(&lexer->current_token);


    if(protocol_lexer_t_is_eof(lexer) || lexer->current == '\n')
    {
        protocol_token_t_create(token, copy("\0"), PROTOCOL_TOKEN_EOF);
        lexer->current_token = (*token);
        return 0;
    }

    if(lexer->current == ' ')
    {
        char* result = NULL;
        int error;
        while(lexer->current == ' ')
        {
            result = concat(result, lexer->current);

            error = protocol_lexer_t_advance(lexer);
            if(error < 0) return error;
        }
    
        protocol_token_t_create(token, result, PROTOCOL_TOKEN_SPACES);
        lexer->current_token = (*token);
    
        return 0;
    }
    else if(lexer->current == '+' && lexer->previous == '\0')
    {
        int error = protocol_lexer_t_advance(lexer);
        if(error < 0) return error;

        protocol_token_t_create(token, copy("+"), PROTOCOL_TOKEN_PLUS);

        lexer->current_token = (*token);
        return 0;
    }
    else if(lexer->current == '-' && lexer->previous == '\0')
    {
        int error = protocol_lexer_t_advance(lexer);
        if(error < 0) return error;
    
        protocol_token_t_create(token, copy("-"), PROTOCOL_TOKEN_MINUS);

        lexer->current_token = (*token);
        return 0;
    }
    else if(lexer->current == '#' && lexer->previous == ' ')
    {
        int error = protocol_lexer_t_advance(lexer);
        if(error < 0) return error;

        protocol_token_t_create(token, copy(" #"), PROTOCOL_TOKEN_TAG);
        lexer->current_token = (*token);
        return 0;
    }

    char* result = NULL;
    protocol_token_type_t type = PROTOCOL_TOKEN_KEYWORD;

    while(
        ! protocol_lexer_t_is_eof(lexer)
        && (lexer->current != ' ' && lexer->current != '\0' && lexer->current != '\n')
    )
    {
        result = concat(result, lexer->current);

        int error = protocol_lexer_t_advance(lexer);
        if(error < 0) return error;
    }

    protocol_token_t_create(token, result, type);

    lexer->current_token = (*token);
    return 0;
}
