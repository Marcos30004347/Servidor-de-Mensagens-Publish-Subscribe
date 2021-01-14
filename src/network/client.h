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

/**
 * Send a @message to the server using the @url endpoint.
 */
void client_t_send(struct client_t* client, char* message);

/**
 * Create a client_t structure.
 */
void client_t_create(struct client_t** client, const char* url, int port);

/**
 * Destroys a client_t structure.
 */
void client_t_destroy(struct client_t* client);

/**
 * Disconnect a client from the server.
 */
void client_t_disconnect(struct client_t* client);


int client_t_receive(struct client_t* client, char* message, int length);

#endif