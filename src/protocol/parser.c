#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* copy_value(protocol_token_t* token)
{
    char* r = (char*)malloc(sizeof(char)*strlen(protocol_token_t_get_value(token)));
    strcpy(r, protocol_token_t_get_value(token));
    return r;
}

char* concat_words(char* str0, char* str1)
{
    unsigned long l0 = strlen(str0);
    unsigned long l1 = strlen(str1);

    char* r = (char*)malloc(sizeof(char)*(l0+l1+2));

    memcpy(r, str0, l0);
    r[l0] = ' ';
    memcpy(r + l0 + 1, str1, l1);
    r[l0 + l1 + 1] = '\0';
    return r;
}

void protocol_ast_t_create(protocol_ast_t** parser)
{
    (*parser) = (protocol_ast_t*)malloc(sizeof(protocol_ast_t));

    (*parser)->ast_node_type            = PROTOCOL_AST_ROOT;
    (*parser)->ast_node_add_value       = NULL;
    (*parser)->ast_node_rem_value       = NULL;
    (*parser)->ast_node_tag_value       = NULL;
    (*parser)->ast_node_phrase_value    = NULL;

    (*parser)->ast_node_child      = NULL;
}

void protocol_ast_t_destroy(protocol_ast_t** parser)
{
    if((*parser)->ast_node_child)
        protocol_ast_t_destroy(&(*parser)->ast_node_child);

    if((*parser)->ast_node_add_value)       free((*parser)->ast_node_add_value);
    if((*parser)->ast_node_rem_value)       free((*parser)->ast_node_rem_value);
    if((*parser)->ast_node_tag_value)       free((*parser)->ast_node_tag_value);
    if((*parser)->ast_node_phrase_value)    free((*parser)->ast_node_phrase_value);
    free(*parser);
    (*parser) = NULL;
}

int protocol_ast_t_parse(protocol_lexer_t* lexer, protocol_ast_t** ast)
{
    protocol_ast_t_create(ast);

    protocol_token_t* token = protocol_lexer_t_get_current_token(lexer);
    
    if(!token || protocol_token_t_get_type(token) == PROTOCOL_TOKEN_EOF)
        return 0;

    // printf("Parsing Protocol\n");

    if(protocol_token_t_get_type(token) == PROTOCOL_TOKEN_PLUS)
    {
        return protocol_ast_t_parse_add(lexer, &(*ast)->ast_node_child);
    }
    else if(protocol_token_t_get_type(token) == PROTOCOL_TOKEN_MINUS)
    {
        return protocol_ast_t_parse_rem(lexer, &(*ast)->ast_node_child);
    }
    else
    {
        return protocol_ast_t_parse_message(lexer, &(*ast)->ast_node_child);
    }

    return 0;
}


int protocol_ast_t_parse_message(protocol_lexer_t* lexer, protocol_ast_t** ast)
{
    protocol_token_t* token = protocol_lexer_t_get_current_token(lexer);
    // printf("Parsing Message\n");
    
    if(protocol_token_t_get_type(token) == PROTOCOL_TOKEN_EOF)
    {
        protocol_lexer_t_get_next_token(lexer);
        return 0;
    }

    protocol_ast_t_create(ast);
    (*ast)->ast_node_type = PROTOCOL_AST_MESS;

    if(protocol_token_t_get_type(token) == PROTOCOL_TOKEN_TAG)
    {
        return protocol_ast_t_parse_tag(lexer, &(*ast)->ast_node_child);
    }
    else if(protocol_token_t_get_type(token) == PROTOCOL_TOKEN_KEYWORD)
    {
        return protocol_ast_t_parse_phrase(lexer, &(*ast)->ast_node_child);
    }

    return 0;
}


int protocol_ast_t_parse_tag(protocol_lexer_t* lexer, protocol_ast_t** ast)
{
    protocol_token_t* token = protocol_lexer_t_get_current_token(lexer);
    // printf("Parsing Tag\n");

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_TAG)
    {
        return -1;
    }

    token = protocol_lexer_t_get_next_token(lexer);

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_KEYWORD)
    {
        return -1;
    }

    protocol_ast_t_create(ast);

    (*ast)->ast_node_tag_value = protocol_token_t_get_value(token);
    (*ast)->ast_node_type      = PROTOCOL_AST_TAG;

    token = protocol_lexer_t_get_next_token(lexer);

    return protocol_ast_t_parse_message(lexer, &(*ast)->ast_node_child);
}


int protocol_ast_t_parse_phrase(protocol_lexer_t* lexer, protocol_ast_t** ast)
{
    // printf("Parsing Phrase\n");
    protocol_token_t* token = protocol_lexer_t_get_current_token(lexer);

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_KEYWORD)
    {
        return -1;
    }
    // printf("Parsing Phrase\n");

    char* phrase            = copy_value(token);
    while (
        (token = protocol_lexer_t_get_next_token(lexer))
        && protocol_token_t_get_type(token) == PROTOCOL_TOKEN_KEYWORD
    ) {
        char* tmp = phrase;
        phrase = concat_words(phrase, protocol_token_t_get_value(token));
        free(tmp);
        // strcat(phrase, copy_value(token));
    }
    // printf("Parsing Phrase\n");

    protocol_ast_t_create(ast);

    (*ast)->ast_node_type = PROTOCOL_AST_PHRASE;
    (*ast)->ast_node_phrase_value = phrase;

    return protocol_ast_t_parse_message(lexer, &(*ast)->ast_node_child);
}



int protocol_ast_t_parse_add(protocol_lexer_t* lexer, protocol_ast_t** ast)
{
    protocol_token_t* token = protocol_lexer_t_get_current_token(lexer);
    // printf("Parsing Add\n");

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_PLUS)
    {
        return -1;
    }

    token = protocol_lexer_t_get_next_token(lexer);

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_KEYWORD)
    {
        return -1;
    }

    protocol_ast_t_create(ast);
    
    (*ast)->ast_node_add_value = copy_value(token);
    (*ast)->ast_node_type      = PROTOCOL_AST_ADD;

    token = protocol_lexer_t_get_next_token(lexer);

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_EOF)
    {
        return -1;
    }

    return 0;
}

int protocol_ast_t_parse_rem(protocol_lexer_t* lexer, protocol_ast_t** ast)
{
    // printf("Parsing Rem\n");

    protocol_token_t* token = protocol_lexer_t_get_current_token(lexer);
    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_MINUS)
    {
        return -1;
    }

    token = protocol_lexer_t_get_next_token(lexer);

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_KEYWORD)
    {
        return -1;
    }

    protocol_ast_t_create(ast);
    (*ast)->ast_node_rem_value = copy_value(token);
    (*ast)->ast_node_type      = PROTOCOL_AST_REM;

    token = protocol_lexer_t_get_next_token(lexer);

    if(protocol_token_t_get_type(token) != PROTOCOL_TOKEN_EOF)
    {
        return -1;
    }

    return 0;
}
