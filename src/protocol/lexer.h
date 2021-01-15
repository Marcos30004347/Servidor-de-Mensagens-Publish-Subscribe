#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct protocol_lexer_t protocol_lexer_t;

int protocol_lexer_t_create(protocol_lexer_t** lexer, const char* source, unsigned long length);
void protocol_lexer_t_destroy(protocol_lexer_t** lexer);
int  protocol_lexer_t_advance(protocol_lexer_t* lexer);

int protocol_lexer_t_is_eof(protocol_lexer_t* lexer);

int protocol_lexer_t_get_next_token(protocol_lexer_t* lexer, protocol_token_t** token);
protocol_token_t* protocol_lexer_t_get_current_token(protocol_lexer_t* lexer);


#endif
