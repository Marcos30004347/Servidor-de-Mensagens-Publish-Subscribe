#ifndef SERVER_H
#define SERVER_H

struct request_t
{
    const char* payload;
    unsigned payload_length;
    struct tcp_server_t* server;
};

struct reply_t
{
    int client;
};

/**
 * The main server structure
 */
struct tcp_server_t;


typedef void(*tcp_server_t_request_handler)(struct request_t req, struct reply_t rep);

/**
 * Allocate and initialize a struct tcp_server_t structure
 */
void tcp_server_t_create(struct tcp_server_t** server);

/**
 * Deallocate and destroys a struct tcp_server_t structure
 */
void tcp_server_t_destroy(struct tcp_server_t* server);

/**
 * Bind a initialized struct tcp_server_t to a port in the host machine
 */
void tcp_server_t_bind_to_port(struct tcp_server_t* server, int port);

// /**
//  * Add one endpoint to the struct tcp_server_t structure
//  */
// void tcp_server_t_add_endpoint(struct tcp_server_t* server, const char* identifier, server_handler handler);

/**
 * Terminate one struct tcp_server_t structure sending a kill message (##kill) to all
 * connected clients.
 */
void tcp_server_t_terminate(struct tcp_server_t* server);

/**
 * Start a struct tcp_server_t structure.
 * This is a blocking operation and it should be called after all endponts
 * and configuration have been made.
 */
void tcp_server_t_start(struct tcp_server_t* server);


void tcp_server_t_set_request_handler(struct tcp_server_t* server, tcp_server_t_request_handler handler);

void tcp_server_t_send(int client, const char* message);

#endif