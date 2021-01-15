#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct protocol_lexer_t protocol_lexer_t;

/**
 * @brief Allocate and creagtes a lexer object.
 * 
 * @param lexer The lexer object.
 * @param source The source that should be lexed.
 * @param length The length of the source protocol.
 * @return Error code.
 */
int protocol_lexer_t_create(protocol_lexer_t** lexer, const char* source, unsigned long length);

/**
 * @brief Destroys and deallocates a lexer object.
 * 
 * @param lexer 
 */
void protocol_lexer_t_destroy(protocol_lexer_t** lexer);

/**
 * @brief Advance in one unit the current character of the lexer.
 * 
 * @param lexer The lexer object.
 * @return Error code.
 */
int  protocol_lexer_t_advance(protocol_lexer_t* lexer);

/**
 * @brief Returns if lexer is in the end of the source protocol.
 * 
 * @param lexer The lexer object.
 * @return Error code.
 */
int protocol_lexer_t_is_eof(protocol_lexer_t* lexer);

/**
 * @brief Returns the next token of the source protocol
 * and updates the lexer current token.
 * 
 * @param lexer The lexer object.
 * @param token The new token object.
 * @return Error code.
 */
int protocol_lexer_t_get_next_token(protocol_lexer_t* lexer, protocol_token_t** token);

/**
 * @brief Returns the current token of the lexer.
 * 
 * @param lexer The lexer object.
 * @return The token object. 
 */
protocol_token_t* protocol_lexer_t_get_current_token(protocol_lexer_t* lexer);


#endif
