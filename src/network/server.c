#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

#include "async.h"
#include "server.h"

#include "hash.h"

struct endpoint_node_t
{
    struct endpoint_node_t* next;
    server_handler value;
    const char* key;
};

struct endpoint_table_t
{
    struct endpoint_node_t* table[128];
};

void endpoint_table_t_create(struct endpoint_table_t** table)
{
    *table = (struct endpoint_table_t*)malloc(sizeof(struct endpoint_table_t));

    for(int i=0; i<128; i++)
    {
        (*table)->table[i] = NULL;
    }
}

void endpoint_table_t_add(struct endpoint_table_t* table, const char* string, server_handler value) {
    int id = hash_string(string)%128;

    if(!table->table[id]) {
        table->table[id] = (struct endpoint_node_t*)malloc(sizeof(struct endpoint_node_t));
        table->table[id]->value = value;
        table->table[id]->key = string;
        return;
    }
    
    struct endpoint_node_t* tail = table->table[id];
    while(tail->next) tail = tail->next;

    tail->next = (struct endpoint_node_t*)malloc(sizeof(struct endpoint_node_t));
    tail->next->value = value;
    tail->next->key = string;
    tail->next->next = NULL;
}


server_handler endpoint_table_t_get_handler(struct endpoint_table_t* table, const char* string)
{
    int id = hash_string(string)%128;
    if(!table->table[id])
    {
        return NULL;
    }
    struct endpoint_node_t* tail = table->table[id];
    while(tail)
    {
        if(strcmp(tail->key, string) == 0)
            return tail->value;
    }

    return NULL;
}


void endpoint_table_t_destroy(struct endpoint_table_t* table)
{
    for(int i=0; i<128; i++)
    {
        while (table->table[i])
        {
            struct endpoint_node_t* tmp = table->table[i];
            table->table[i] = table->table[i]->next;
            free(tmp);
        }
        
    }
    free(table);

}

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

struct server_endpoint
{
    char id;
    int active;
    server_handler handler;
};

struct thread_data
{
    struct server_t* server;
    int connfd;
};

struct server_t
{
    int server_fd;
    struct sockaddr_in address;
    
    // One possible handler for each ASCII character
    struct endpoint_table_t* endpoints;

    // Connections List
    struct connection_node_t* connections;

    struct mutex_t* lock;

    struct mutex_t* conn_lock;

    struct thread_t* connect_server;

    int state;
};


void remove_conn_list_node_t(struct connection_node_t** node)
{
    if((*node)->prev) (*node)->prev->next = (*node)->next;
    if((*node)->next) (*node)->next->prev = (*node)->prev;

    thread_t_destroy((*node)->thread);
    free((*node));
}

server_handler server_t_get_handler_for_endpoint(struct server_t* server, const char* endpoint) {
    return endpoint_table_t_get_handler(server->endpoints, endpoint);
}


void* server_t_client_handler(void* c) {
    struct thread_data* data = (struct thread_data*)c;
    int connfd = data->connfd;
    char request_message[500]; 

    while(read(connfd, request_message, sizeof(request_message)))
    {

        char* endpoint = NULL;
        char* message = NULL;

        endpoint = strtok(request_message, "\r\n");
        message = strtok(NULL, "\r\n");

        if(!endpoint || !message) continue;

        server_handler handler = server_t_get_handler_for_endpoint(data->server, endpoint);
        
        if(!handler) {
            char reply_message[500];
            sprintf(reply_message," No endpoint %s found on the server!", endpoint);
            reply_t_send(data->connfd, reply_message);
            continue;
        }

        struct request_t req;
        struct reply_t rep;
    
        req.endpoint = endpoint;
        req.message = message;   
        req.server = data->server;

        rep.client = data->connfd;

        mutex_t_lock(data->server->lock);
        handler(req, rep);
        mutex_t_unlock(data->server->lock);

        bzero(request_message, 500); 
    }

    free(data);
    return NULL;
}



struct connection_t server_t_accept_connection(struct server_t* server) {
    struct connection_t conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

void server_t_hold_connection(struct server_t* server, struct connection_t connection) {
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

    thread_t_create(&thread, server_t_client_handler, data);

    conn->thread = thread;
    conn->client = connection.client_fd;
}

void* accept_connection_thread(void* args)
{

    struct server_t* server = (struct server_t*)(args);
    struct connection_t connection = server_t_accept_connection(server);

    while(connection.client_fd != -1)
    {
        mutex_t_lock(server->conn_lock);
        server_t_hold_connection(server, connection);
        mutex_t_unlock(server->conn_lock);

        connection = server_t_accept_connection(server);
    }

    return NULL;
}

void server_t_create(struct server_t** server)
{
    *server = (struct server_t*)malloc(sizeof(struct server_t));
    struct server_t* serv = *server;
    mutex_t_create(&serv->lock);
    mutex_t_create(&serv->conn_lock);

    mutex_t_lock(serv->lock);
    serv->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv->state = 1;

    serv->connections = NULL;
    endpoint_table_t_create(&serv->endpoints);

    if (serv->server_fd == -1) { 
        printf("socket creation failed...\n"); 
        mutex_t_unlock(serv->lock);
        exit(0); 
    }


    mutex_t_unlock(serv->lock);
}




void server_t_bind_to_port(struct server_t* server, int port)
{
    server->address.sin_family = AF_INET; 
    server->address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server->address.sin_port = htons(port); 

    if ((bind(server->server_fd, (struct sockaddr*)&server->address, sizeof(server->address))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
}

void server_t_add_endpoint(struct server_t* server, const char* identifier, server_handler handler) 
{
    endpoint_table_t_add(server->endpoints, identifier, handler);
}


void server_t_terminate(struct server_t* server) {

    struct connection_node_t * tmp = server->connections;

    while(tmp)
    {
        reply_t_send(tmp->client, "##kill");
        tmp = tmp->prev;
    }

    // mutex_t_lock(server->conn_lock);
    // thread_t_cancel(server->connect_server);
    close(server->server_fd);
    // mutex_t_unlock(server->conn_lock);
}

void server_t_destroy(struct server_t* server)
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

    endpoint_table_t_destroy(server->endpoints);
    // close(server->server_fd);
    mutex_t_destroy(server->lock);
    mutex_t_destroy(server->conn_lock);
    
    free(server);
}

void server_t_start(struct server_t* server)
{
    // printf("starting server_t \n");

    if ((listen(server->server_fd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    }

    thread_t_create(&server->connect_server, accept_connection_thread, server);
    thread_t_join(server->connect_server);
}
