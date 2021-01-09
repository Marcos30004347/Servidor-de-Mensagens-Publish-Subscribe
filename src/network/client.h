#ifndef CLIENT_H
#define CLIENT_H

/**
 * Client main structure
 */
struct client_t;

/**
 * The client receive message handler
 */
typedef void(*client_handler)(struct client_t*, char*);

void client_t_send(struct client_t* client, const char* url, char* message);

void client_t_create(struct client_t** client, client_handler handler, const char* url, int port);

void client_t_destroy(struct client_t* client);

#endif