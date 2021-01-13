#ifndef SERVER_H
#define SERVER_H

#include "request.h"


/**
 * The main server structure
 */
struct server_t;

/**
 * Allocate and initialize a server_t structure
 */
void server_t_create(struct server_t** server);

/**
 * Deallocate and destroys a server_t structure
 */
void server_t_destroy(struct server_t* server);

/**
 * Bind a initialized server_t to a port in the host machine
 */
void server_t_bind_to_port(struct server_t* server, int port);

/**
 * Add one endpoint to the server_t structure
 */
void server_t_add_endpoint(struct server_t* server, const char* identifier, server_handler handler);

/**
 * Terminate one server_t structure sending a kill message (##kill) to all
 * connected clients.
 */
void server_t_terminate(struct server_t* server);

/**
 * Start a server_t structure.
 * This is a blocking operation and it should be called after all endponts
 * and configuration have been made.
 */
void server_t_start(struct server_t* server);


#endif