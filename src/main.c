#include "protocol/parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    protocol_lexer_t* lexer = NULL;
    protocol_lexer_t_create(&lexer, "+teste", strlen("+teste"));

    protocol_ast_t* ast = NULL;

    int error = protocol_ast_t_parse(lexer, &ast);
    
    while (ast)
    {
        printf("type %i\n", ast->ast_node_type);
        ast = ast->ast_node_child;
    }
    

    printf("error %i\n", error);

    return 0;
}