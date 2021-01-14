#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

#include "async.h"
#include "server.h"

struct connection_node_t {
    struct thread_t* thread;
    int client;
    struct connection_node_t* next;
    struct connection_node_t* prev;
};

struct connection_t
{
    int client_fd;
    struct sockaddr_in client_address;
};

struct thread_data
{
    struct tcp_server_t* server;
    int connfd;
};


struct tcp_server_t
{
    int server_fd;
    struct sockaddr_in address;

    struct connection_node_t* connections;

    struct mutex_t* lock;

    struct mutex_t* conn_lock;

    struct thread_t* connect_server;

    tcp_server_t_request_handler request_handler;

    int state;
};

void* tcp_server_t_client_handler(void* c) {
    struct thread_data* data = (struct thread_data*)c;
    int connfd = data->connfd;
    char request_message[500]; 

    while(read(connfd, request_message, sizeof(request_message)))
    {
        struct request_t req;
        struct reply_t rep;
    
        req.payload_length = strlen(request_message);
        req.payload = request_message;   
        req.server = data->server;

        rep.client = data->connfd;

        mutex_t_lock(data->server->lock);
        data->server->request_handler(req, rep);
        mutex_t_unlock(data->server->lock);

        bzero(request_message, 500); 
    }

    free(data);
    return NULL;
}
void tcp_server_t_hold_connection(struct tcp_server_t* server, struct connection_t connection) {
    struct thread_t* thread = NULL;

    struct thread_data* data =(struct thread_data*) malloc(sizeof(struct thread_data));

    data->connfd = connection.client_fd;
    data->server = server;

    struct connection_node_t* conn = (struct connection_node_t*)malloc(sizeof(struct connection_node_t));

    if(!data->server->connections)
    {
        data->server->connections = conn;
    }
    else
    {
        data->server->connections->next = conn;
        conn->prev = data->server->connections;
        data->server->connections = conn;
    }

    thread_t_create(&thread, tcp_server_t_client_handler, data);

    conn->thread = thread;
    conn->client = connection.client_fd;
}

void tcp_server_t_create(struct tcp_server_t** server)
{
    *server = (struct tcp_server_t*)malloc(sizeof(struct tcp_server_t));
    struct tcp_server_t* serv = *server;
    mutex_t_create(&serv->lock);
    mutex_t_create(&serv->conn_lock);

    serv->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    serv->request_handler = NULL;
    serv->state = 1;

    serv->connections = NULL;
    // endpoint_table_t_create(&serv->endpoints);

    if (serv->server_fd == -1) { 
        printf("socket creation failed...\n"); 
        mutex_t_unlock(serv->lock);
        exit(0); 
    }
}

void tcp_server_t_destroy(struct tcp_server_t* server)
{
    struct connection_node_t * tmp;
    mutex_t_lock(server->conn_lock);
    while (server->connections)
    {
        tmp = server->connections;
    
        server->connections = server->connections->prev;
        if(server->connections)
            server->connections->next = NULL;

        thread_t_destroy(tmp->thread);
        free(tmp);
    }

    thread_t_destroy(server->connect_server);

    mutex_t_unlock(server->conn_lock);

    mutex_t_destroy(server->lock);
    mutex_t_destroy(server->conn_lock);
    
    free(server);
}

struct connection_t tcp_server_t_accept_connection(struct tcp_server_t* server) {
    struct connection_t conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

void* accept_connection_thread(void* args)
{
    struct tcp_server_t* server = (struct tcp_server_t*)(args);
    struct connection_t connection = tcp_server_t_accept_connection(server);

    while(connection.client_fd != -1)
    {
        mutex_t_lock(server->conn_lock);
        tcp_server_t_hold_connection(server, connection);
        mutex_t_unlock(server->conn_lock);

        connection = tcp_server_t_accept_connection(server);
    }

    return NULL;
}

void tcp_server_t_start(struct tcp_server_t* server)
{
    if ((listen(server->server_fd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    }

    thread_t_create(&server->connect_server, accept_connection_thread, server);
    thread_t_join(server->connect_server);
}

void tcp_server_t_terminate(struct tcp_server_t* server) {

    struct connection_node_t * tmp = server->connections;

    while(tmp)
    {
        tcp_server_t_send(tmp->client, "##kill");
        tmp = tmp->prev;
    }

    close(server->server_fd);
}







void tcp_server_t_bind_to_port(struct tcp_server_t* server, int port)
{
    server->address.sin_family = AF_INET; 
    server->address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server->address.sin_port = htons(port); 

    if ((bind(server->server_fd, (struct sockaddr*)&server->address, sizeof(server->address))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
}

void tcp_server_t_set_request_handler(struct tcp_server_t* server, tcp_server_t_request_handler handler) 
{
    server->request_handler = handler;
}

void tcp_server_t_send(int client, const char* message)
{
    write(client, message, strlen(message)); 
}