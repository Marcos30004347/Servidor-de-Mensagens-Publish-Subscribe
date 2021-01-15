#include "protocol/parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void printAST(protocol_ast_t* ast)
{
    while (ast)
    {
        if(ast->ast_node_type == PROTOCOL_AST_ADD)
        {
            printf("ADDING TAG %s ", ast->ast_node_add_value);
        }
        if(ast->ast_node_type == PROTOCOL_AST_TAG)
        {
            printf("TAG %s ", ast->ast_node_tag_value);
        }  
        if(ast->ast_node_type == PROTOCOL_AST_REM)
        {
            printf("REMMOVING TAG %s ", ast->ast_node_rem_value);
        }    
        if(ast->ast_node_type == PROTOCOL_AST_PHRASE)
        {
            printf("MESSAGE %s ", ast->ast_node_phrase_value);
        }
        ast = ast->ast_node_child;
    }
    printf("\n");
}

void parse(const char* src)
{
    protocol_lexer_t* lexer = NULL;
    protocol_ast_t* ast = NULL;

    protocol_lexer_t_create(&lexer, src, strlen(src));

    int error = protocol_ast_t_parse(lexer, &ast);
    if(error)
    {
        printf("Syntax Error!\n");
        return;
    }
    printAST(ast);
}

int main()
{
    parse("+te^ste\n");
    parse("+te~ste\n");


    parse("+teste\n");
    parse("+C#\n");
    parse("-teste\n");
    parse("teste #teste\n");
    parse(" #teste hahaha\n");
    parse("teste #teste1 #teste2\n");
    parse("oi #teste oi #teste\n");
    parse("testando testando #teste ah nao #teste\n");
    parse("#testando ctest#ando #teste ah nao #teste\n");
    parse("#testando#test#ando #teste ah nao #teste\n");
    parse("kkkkk #C#\n");

    return 0;
}