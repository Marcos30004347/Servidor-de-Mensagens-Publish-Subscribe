#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network/server.h"
#include "channel_table.h"

struct tcp_server_t* server = NULL;
struct channel_table_t* channels = NULL;

void channel_listen(char* payload, struct reply_t reply)
{
    char reply_message[500] = {'\0'};
    char channel[500] = {'\0'};
    memcpy(channel, &payload[1], strlen(payload) - 1);

    if(channel_table_t_get_client(channels, channel, reply.client) != NULL)
    {
        sprintf(reply_message,"already subscribed +%s", channel);
        tcp_server_t_send(reply.client, reply_message);
        return;
    }

    channel_table_t_add(channels, channel, reply.client);
    sprintf(reply_message,"subscribed +%s", channel);
    tcp_server_t_send(reply.client, reply_message);
}

void channel_mute(char* payload, struct reply_t reply)
{
    char reply_message[500];
    char channel[500] = {'\0'};
    memcpy(channel, &payload[1], strlen(payload) - 1);

    struct client_node_t * cli = channel_table_t_get_client(channels, channel, reply.client);

    if(cli == NULL)
    {
        sprintf(reply_message,"not subscribed -%s", channel);
        tcp_server_t_send(reply.client, reply_message);
        return;
    }

    channel_table_t_remove(channels, channel, reply.client);
    sprintf(reply_message,"unsubscribed -%s", channel);
    tcp_server_t_send(reply.client, reply_message);
}


void broadcast(char* payload, struct reply_t reply)
{
    char* message = payload;
 
    char* channel = NULL;
    char* text = NULL;

    text = strtok(message, "#");
    channel = strtok(NULL, "#");
    while(channel != NULL) {

        struct client_node_t* client = channel_table_t_get_channel(channels, channel);

        while(client) {
            tcp_server_t_send(client->client_id, text);
            client = client->prev;
        }

        channel = strtok(NULL, "#");
    }
}

void server_handler(struct request_t request, struct reply_t reply)
{
    if(strcmp(request.payload, "##kill") == 0)
    {
        tcp_server_t_terminate(request.server);
    }
    else if(request.payload[0] == '+')
    {
        channel_listen((char*)request.payload, reply);
    }
    else if(request.payload[0] == '-')
    {
        channel_mute((char*)request.payload, reply);
    }
    else 
    {
        broadcast((char*)request.payload, reply);
    }
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