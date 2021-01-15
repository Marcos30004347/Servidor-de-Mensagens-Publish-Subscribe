#ifndef TOKEN_H
#define TOKEN_H

typedef struct protocol_token_t protocol_token_t;

typedef enum {
    PROTOCOL_TOKEN_NONE     = 1,
    PROTOCOL_TOKEN_KEYWORD  = 2,
    PROTOCOL_TOKEN_PLUS     = 3,
    PROTOCOL_TOKEN_MINUS    = 4,
    PROTOCOL_TOKEN_TAG      = 5,
    PROTOCOL_TOKEN_EOF      = 6,
} protocol_token_type_t;



/**
 * @brief Creates a token object.
 * 
 * @param token The token object.
 * @param value The value that the tolken should have.
 * @param type The type of the token.
 */
void protocol_token_t_create(protocol_token_t** token, char* value, protocol_token_type_t type);

/**
 * @brief Destroys and deallocates a token object.
 * 
 * @param token The token object.
 */
void protocol_token_t_destroy(protocol_token_t** token);

/**
 * @brief Returns the type of the token.
 * 
 * @param token The token object.
 * @return protocol_token_type_t 
 */
protocol_token_type_t protocol_token_t_get_type(protocol_token_t* token);

/**
 * @brief Returns the value of the tolken.
 * 
 * @param token The token object.
 * @return Pointer to the string value.
 */
const char* protocol_token_t_get_value(protocol_token_t* token);


#endif
