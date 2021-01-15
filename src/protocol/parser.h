#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

//S -> (M*( \#'P)\*)\* | +P | -P | P;
//Q -> , | . | ? | ! | : | ; | + | - | * | / | = | @ | # | $ | $ | % | ( | ) | { | };
//L -> [a-z] | [A-z] | [0-9];
//P -> L* | QL | LPL | LP;
//M -> P( P)*;

typedef enum {
    PROTOCOL_AST_ROOT   = 0,
    PROTOCOL_AST_TAG    = 1,
    PROTOCOL_AST_ADD    = 2,
    PROTOCOL_AST_REM    = 3,
    PROTOCOL_AST_PHRASE = 4,
    PROTOCOL_AST_MESS   = 5,
} protocol_ast_node_type;

struct protocol_ast_t
{
    protocol_ast_node_type  ast_node_type;
    char*                   ast_node_tag_value;
    char*                   ast_node_add_value;
    char*                   ast_node_rem_value;
    char*                   ast_node_phrase_value;
    struct protocol_ast_t*  ast_node_child;
};

typedef struct protocol_ast_t protocol_ast_t;

int protocol_ast_t_parse(protocol_lexer_t* lexer, protocol_ast_t** ast);
int protocol_ast_t_parse_tag(protocol_lexer_t* lexer, protocol_ast_t** ast);
int protocol_ast_t_parse_add(protocol_lexer_t* lexer, protocol_ast_t** ast);
int protocol_ast_t_parse_rem(protocol_lexer_t* lexer, protocol_ast_t** ast);
int protocol_ast_t_parse_phrase(protocol_lexer_t* lexer, protocol_ast_t** ast);
int protocol_ast_t_parse_message(protocol_lexer_t* lexer, protocol_ast_t** ast);

void protocol_ast_t_destroy(protocol_ast_t** parser);

#endif
