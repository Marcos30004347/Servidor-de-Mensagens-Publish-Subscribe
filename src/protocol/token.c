#include "token.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct protocol_token_t
{
    protocol_token_type_t   type;
    char*                   value;
};


void protocol_token_t_create(protocol_token_t** token, char* value, protocol_token_type_t type)
{
    (*token) = (protocol_token_t*)malloc(sizeof(protocol_token_t));
    (*token)->value = value;
    (*token)->type = type;
}

void protocol_token_t_destroy(protocol_token_t** token)
{
    if((*token)->value) free((*token)->value);
    free((*token));
    (*token) = NULL;
}

protocol_token_type_t protocol_token_t_get_type(protocol_token_t* token)
{
    return token->type;
}

const char* protocol_token_t_get_value(protocol_token_t* token)
{
    return token->value;
}