#ifndef NETWORK_TCP_SERVER_H
#define NETWORK_TCP_SERVER_H

#define TCP_SERVER_SYNC 1 << 2
#define TCP_SERVER_MAX_PAYLOAD_LENGTH 1224

/**
 * @brief The TCP Server structure.
 * 
 */
struct tcp_server_t;

/**
 * @brief This structure holds the information that was
 * send by one connected client and the server that
 * receive it.
 */
struct request_t
{
    /**
     * @brief The request payload.
     */
    const char* payload;

    /**
     * @brief The request payload length.
     */
    unsigned payload_length;

    /**
     * @brief The tcp_server responsable for handling the request.
     */
    struct tcp_server_t* server;
};

/**
 * @brief This structure holds the information about who
 * shoud a message be send. This is always used in the a
 * same context as a request_t structure, and in that
 * situation it always holds the id of the client that
 * send the payload inside the request_t structure. 
 */
struct reply_t
{
    /**
     * @brief The id of one client.
     */
    int client_id;
};


/**
 * @brief This is a type that represents a pointer
 * to a function that will handle a payload.
 */
typedef void(*tcp_server_t_request_handler)(struct request_t req, struct reply_t rep);

/**
 * @brief This function allocates and initialize a TCP Server.
 * 
 * @param server A pointer tho the tcp_server_t that should be initialized.
 * @param flags The flags that will define the behavioud of the server.
 * 
 * @flags:
 *   * TCP_SERVER_SYNC - tels the server that it should process
 *   incoming requests in a syncronized way.
 */
void tcp_server_t_create(struct tcp_server_t** server, int flags);

/**
 * @brief Deallocate and destroys a tcp_server_t structure.
 * 
 * @param server The server that should be destroyed.
 */
void tcp_server_t_destroy(struct tcp_server_t* server);

/**
 * @brief Bind a initialized struct tcp_server_t to a port in the host machine
 * 
 * @param server The server structure.
 * @param port The port that the server should listen.
 */
void tcp_server_t_bind_to_port(struct tcp_server_t* server, int port);

/**
 * @brief Stops a server.
 * 
 * @param server The server that should stop running.
 */
void tcp_server_t_terminate(struct tcp_server_t* server);

/**
 * @brief Start a server making it accept connection.
 * 
 * @param server The server that should start executing.
 */
void tcp_server_t_start(struct tcp_server_t* server);

/**
 * @brief Set the request handler of a server.
 * 
 * @param server The server object.
 * @param handler The handler function pointer.
 */
void tcp_server_t_set_request_handler(struct tcp_server_t* server, tcp_server_t_request_handler handler);

/**
 * @brief Sends a message to a connected tcp client.
 * 
 * @param client The client that sould receive the message.
 * @param message The message that sould be send.
 */
void send_message_to_client(int client, const char* message);

#endif