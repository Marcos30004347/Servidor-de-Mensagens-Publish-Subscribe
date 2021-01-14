#ifndef CLIENT_H
#define CLIENT_H

/**
 * Client main structure
 */
struct tcp_client_t;

/**
 * The client receive message handler
 */
typedef void(*client_handler)(struct tcp_client_t*, char*);

/**
 * Send a @message to the server using the @url endpoint.
 */
void tcp_client_t_send(struct tcp_client_t* client, char* message);

/**
 * Create a tcp_client_t structure.
 */
void tcp_client_t_create(struct tcp_client_t** client, const char* url, int port);

/**
 * Destroys a tcp_client_t structure.
 */
void tcp_client_t_destroy(struct tcp_client_t* client);

/**
 * Disconnect a client from the server.
 */
void tcp_client_t_disconnect(struct tcp_client_t* client);


int tcp_client_t_receive(struct tcp_client_t* client, char* message, int length);

#endif