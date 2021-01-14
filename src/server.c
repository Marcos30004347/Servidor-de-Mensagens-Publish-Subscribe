#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network/tcp_server.h"

#include "channel_table.h"
#include "burned_table.h"

struct tcp_server_t* server = NULL;
struct channel_table_t* channels = NULL;

void register_to_channel(char* payload, int client)
{
    char reply_message[500] = {'\0'};
    char channel[500] = {'\0'};
    memcpy(channel, &payload[1], strlen(payload) - 1);

    if(channel_table_t_get_client(channels, channel, client) != NULL)
    {
        sprintf(reply_message,"already subscribed +%s", channel);
        tcp_server_t_send(client, reply_message);
        return;
    }

    channel_table_t_add(channels, channel, client);
    sprintf(reply_message,"subscribed +%s", channel);
    tcp_server_t_send(client, reply_message);
}

void unregister_from_channel(char* payload, int client)
{
    char reply_message[500];
    char channel[500] = {'\0'};
    memcpy(channel, &payload[1], strlen(payload) - 1);

    struct client_node_t * cli = channel_table_t_get_client(channels, channel, client);

    if(cli == NULL)
    {
        sprintf(reply_message,"not subscribed -%s", channel);
        tcp_server_t_send(client, reply_message);
        return;
    }

    channel_table_t_remove(channels, channel, client);
    sprintf(reply_message,"unsubscribed -%s", channel);
    tcp_server_t_send(client, reply_message);
}


void broadcast_in_channel(char* payload, int client)
{
    char* message = payload;

    struct burned_table_t* table = NULL;
    burned_table_t_create(&table);

    char* channel = NULL;
    char* text = NULL;

    text = strtok(message, " #");

    unsigned long message_length = strlen(text);

    if(message_length > 500) 
    {
        tcp_server_t_send(client, "ERROR: não é possivel enviar mensagens maiores que 500 caracteres!");
    }

    for(int i=0; i< message_length; i++) {
        if(message[i] < 21 || message[i] > 126) tcp_server_t_send(client, "##kill");
        else if(message[i] == '^')              tcp_server_t_send(client, "##kill");
        else if(message[i] == '~')              tcp_server_t_send(client, "##kill");
        else if(message[i] == '"')              tcp_server_t_send(client, "##kill");
        else if(message[i] == '\'')             tcp_server_t_send(client, "##kill");
    }


    channel = strtok(NULL, " #");

    while(channel != NULL) {
        struct client_node_t* client = channel_table_t_get_channel(channels, channel);
        
        while(client) {
            if(burned_table_t_get_client(table, client->client_id)) continue;
            burned_table_t_add(table, client->client_id);
            tcp_server_t_send(client->client_id, text);
            client = client->prev;
        }

        channel = strtok(NULL, "#");
    }

    burned_table_t_destroy(table);
}

void server_handler(struct request_t request, struct reply_t reply)
{
    if(strcmp(request.payload, "##kill") == 0)  tcp_server_t_terminate(request.server);
    else if(request.payload[0] == '+')          register_to_channel((char*)request.payload, reply.client);
    else if(request.payload[0] == '-')          unregister_from_channel((char*)request.payload, reply.client);
    else                                        broadcast_in_channel((char*)request.payload, reply.client);
}

int main(int argc, char *argv[]) {
    if(argc < 2) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);

    channel_table_t_create(&channels);

    tcp_server_t_create(&server);
    tcp_server_t_bind_to_port(server, port);
    tcp_server_t_set_request_handler(server, server_handler);

    // Start the server
    tcp_server_t_start(server);

    channel_table_t_destroy(channels);
    tcp_server_t_destroy(server);
}