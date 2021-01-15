#include "tcp_server.h"

#include "async.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

struct connection_node_t {
    int                             client;
    struct thread_t*                thread;
    
    struct connection_node_t*       next;
    struct connection_node_t*       prev;
};

struct connection_t {
    int                             client_fd;
    struct sockaddr_in              client_address;
};

struct thread_data {
    struct tcp_server_t*            server;
    int                             connfd;
};

struct tcp_server_t {
    int                             flags;
    int                             server_fd;
    const char*                     kill_message;
    struct sockaddr_in              address;
    struct connection_node_t*       connections;
    struct mutex_t*                 req_lock;
    tcp_server_t_request_handler    request_handler;
};

void* tcp_server_t_client_handler(void* c)
{
    struct thread_data* data = (struct thread_data*)c;

    struct tcp_server_t* server = data->server;
    int connfd = data->connfd;

    char request[TCP_SERVER_MAX_PAYLOAD_LENGTH] = {'\0'};
    char message[TCP_SERVER_MAX_PAYLOAD_LENGTH] = {'\0'};
    char payload[TCP_SERVER_MAX_PAYLOAD_LENGTH] = {'\0'};


    while(read(connfd, request, TCP_SERVER_MAX_PAYLOAD_LENGTH))
    {
        if(strlen(message) + strlen(request) > TCP_SERVER_MAX_PAYLOAD_LENGTH)
        {
            tcp_server_t_disconnect_client(server, connfd);
            break;
        }
    
        strcat(message, request);

        unsigned long message_length = strlen(message);
        bzero(request, TCP_SERVER_MAX_PAYLOAD_LENGTH); 

        for(int i=0; i<message_length; i++)
        {
            if(message[i] == '\n')
            {
                bzero(payload, TCP_SERVER_MAX_PAYLOAD_LENGTH);
                memcpy(payload, message, i+1);
                struct request_t req;
                struct reply_t rep;
            
                req.payload_length  = strlen(payload);
                req.payload         = payload;   
                req.server          = server;

                rep.client_id       = connfd;

                if(server->flags & TCP_SERVER_SYNC) mutex_t_lock(server->req_lock);
                server->request_handler(req, rep);
                if(server->flags & TCP_SERVER_SYNC) mutex_t_unlock(server->req_lock);

                bzero(message, i);
                memmove(message, message + i + 1, message_length - i - 1);
    
                break;
            }
        }
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

void tcp_server_t_create(struct tcp_server_t** server, int flags)
{
    *server = (struct tcp_server_t*)malloc(sizeof(struct tcp_server_t));
    mutex_t_create(&(*server)->req_lock);

    (*server)->kill_message = NULL;
    (*server)->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    (*server)->request_handler = NULL;
    (*server)->flags = flags;
    (*server)->connections = NULL;

    if ((*server)->server_fd == -1) { 
        printf("[ERRORÇ: criacao do socket falhou!\n"); 
        mutex_t_unlock((*server)->req_lock);
        exit(0); 
    }
}

void tcp_server_t_destroy(struct tcp_server_t* server)
{
    struct connection_node_t * tmp;
    while (server->connections)
    {
        tmp = server->connections;

        server->connections = server->connections->prev;
        if(server->connections)
            server->connections->next = NULL;

        thread_t_destroy(tmp->thread);
        free(tmp);
    }

    mutex_t_destroy(server->req_lock);
    free(server);
}

struct connection_t tcp_server_t_accept_connection(struct tcp_server_t* server)
{
    struct connection_t conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

void tcp_server_t_start(struct tcp_server_t* server)
{
    if ((listen(server->server_fd, 5)) != 0) { 
        printf("[ERROR]: Nao foi possivel escutar\n"); 
        exit(0); 
    }

    struct connection_t connection = tcp_server_t_accept_connection(server);

    while(connection.client_fd != -1)
    {
        tcp_server_t_hold_connection(server, connection);
        connection = tcp_server_t_accept_connection(server);
    }
}

void tcp_server_t_terminate(struct tcp_server_t* server)
{
    struct connection_node_t * tmp = server->connections;

    while(tmp)
    {
        if(server->kill_message)
            send_message_to_client(tmp->client, server->kill_message);
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

void send_message_to_client(int client, const char* message)
{
    write(client, message, strlen(message)); 
}

void tcp_server_t_disconnect_client(struct tcp_server_t* server, int client)
{
    struct connection_node_t ** connections = &server->connections;
    while ((*connections) && (*connections)->client!=client)
        (*connections) = (*connections)->next;
    
    if((*connections) && (*connections)->client == client)
    {
        if((*connections)->prev) (*connections)->prev->next = (*connections)->next;
        if((*connections)->next) (*connections)->next->prev = (*connections)->prev;

        if(server->kill_message)
            send_message_to_client(client, server->kill_message);

        thread_t_destroy((*connections)->thread);
        free((*connections));
        *connections = NULL;
    }
}

void tcp_server_t_set_disconnect_message(struct tcp_server_t* server, const char* message)
{
    server->kill_message = message;
}
