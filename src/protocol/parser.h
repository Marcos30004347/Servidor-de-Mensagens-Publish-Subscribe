#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

/**
 * @brief The type of the AST node.
 */
typedef enum {
    PROTOCOL_AST_ROOT   = 0,
    PROTOCOL_AST_TAG    = 1,
    PROTOCOL_AST_ADD    = 2,
    PROTOCOL_AST_REM    = 3,
    PROTOCOL_AST_PHRASE = 4,
    PROTOCOL_AST_MESS   = 5,
} protocol_ast_node_type;

/**
 * @brief The protocol AST type.
 */
struct protocol_ast_t
{
    protocol_ast_node_type  ast_node_type;
    char*                   ast_node_tag_value;
    char*                   ast_node_add_value;
    char*                   ast_node_rem_value;
    char*                   ast_node_phrase_value;
    struct protocol_ast_t*  ast_node_child;
};

/**
 * @brief The protocol AST node data structure.
 */
typedef struct protocol_ast_t protocol_ast_t;

/**
 * @brief Parse the Protocol using the lexer.
 * 
 * @param lexer The lexer object.
 * @param ast The AST object.
 * @return Error code.
 */
int protocol_ast_t_parse(protocol_lexer_t* lexer, protocol_ast_t** ast);


/**
 * @brief Parse a Protocol Tag using the lexer.
 * 
 * @param lexer The lexer object.
 * @param ast The AST object.
 * @return Error code.
 */
int protocol_ast_t_parse_tag(protocol_lexer_t* lexer, protocol_ast_t** ast);

/**
 * @brief Parse a Protocol Add using the lexer.
 * 
 * @param lexer The lexer object.
 * @param ast The AST object.
 * @return Error code.
 */
int protocol_ast_t_parse_add(protocol_lexer_t* lexer, protocol_ast_t** ast);

/**
 * @brief Parse a Protocol Sub using the lexer.
 * 
 * @param lexer The lexer object.
 * @param ast The AST object.
 * @return Error code.
 */
int protocol_ast_t_parse_rem(protocol_lexer_t* lexer, protocol_ast_t** ast);

/**
 * @brief Parse a Protocol phrase.
 * 
 * @param lexer The lexer object.
 * @param ast The AST object.
 * @return Error code.
 */
int protocol_ast_t_parse_phrase(protocol_lexer_t* lexer, protocol_ast_t** ast);

/**
 * @brief Parse a Protocol message.
 * 
 * @param lexer The lexer object.
 * @param ast The AST object.
 * @return Error code.
 */
int protocol_ast_t_parse_message(protocol_lexer_t* lexer, protocol_ast_t** ast);


/**
 * @brief Destroys and deallocate a protocol_ast_t object.
 * 
 * @param parser The parser object.
 */
void protocol_ast_t_destroy(protocol_ast_t** parser);


/**
 * @brief Print the abstract syntax tree of the protocol.
 * 
 * @param ast 
 */
void protocol_ast_t_print(protocol_ast_t* ast);


#endif
