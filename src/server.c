#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "network/tcp_server.h"

#include "channel_table.h"
#include "burned_table.h"

struct tcp_server_t* server = NULL;
struct channel_table_t* channels = NULL;

void register_to_channel(const char* payload, unsigned int payload_length, int client)
{
    char reply_message[500] = {'\0'};
    char channel[500] = {'\0'};

    memcpy(channel, &payload[1], payload_length - 1);

    if(channel_table_t_get_client(channels, channel, client) != NULL)
    {
        sprintf(reply_message,"already subscribed +%s", channel);
        send_message_to_client(client, reply_message);
        return;
    }

    channel_table_t_add(channels, channel, client);
    sprintf(reply_message,"subscribed +%s", channel);

    send_message_to_client(client, reply_message);
}

void unregister_from_channel(const char* payload, unsigned int payload_length, int client)
{
    char reply_message[500];

    char channel[500] = {'\0'};
    memcpy(channel, &payload[1], payload_length - 1);

    struct client_node_t * cli = channel_table_t_get_client(channels, channel, client);

    if(cli == NULL)
    {
        sprintf(reply_message,"not subscribed -%s", channel);
        send_message_to_client(client, reply_message);
        return;
    }

    channel_table_t_remove(channels, channel, client);

    sprintf(reply_message, "unsubscribed -%s", channel);
    send_message_to_client(client, reply_message);
}


void broadcast_in_channel(const char* payload, unsigned payload_len, int client)
{
    struct burned_table_t* table = NULL;
    burned_table_t_create(&table);

    char message[TCP_SERVER_MAX_PAYLOAD_LENGTH] = {'\0'};
    
    memcpy(message, payload, payload_len);

    char* channel = NULL;
    char* text = NULL;

    text = strtok(message, "#");

    unsigned long message_length = payload_len;

    LOG("Verifying payload!\n");
    for(int i=0; i< strlen(text); i++) {
        LOG("%c = %i\n", message[i], message[i]);
        if(
            message[i] < 21 || message[i] > 126
            || message[i] == '^'
            || message[i] == '~'
            || message[i] == '"'
            || message[i] == '\''
            || message[i] == '\n'
            || message[i] == '\0'
        )
        {
            send_message_to_client(client, "##kill");
            return;
        }
    }

    channel = strtok(NULL, "#");

    while(channel != NULL) {
        LOG("[SENDING PAYLOAD]: '%s'(%i) on channel '%s'", payload, payload_len, channel);
        struct client_node_t* client = channel_table_t_get_channel(channels, channel);
        
        while(client) {
            if(burned_table_t_get_client(table, client->client_id))
                continue;
            
            burned_table_t_add(table, client->client_id);
            
            send_message_to_client(client->client_id, payload);
            
            client = client->prev;
        }

        channel = strtok(NULL, "#");
    }

    burned_table_t_destroy(table);
}

void server_handler(struct request_t request, struct reply_t reply)
{
    if(request.payload_length > 500) 
    {
        LOG("Client %i send payload with forbiden length of %i!\n", request.payload_length);
        send_message_to_client(reply.client_id, "##kill");
        return;
    }

    LOG("[PAYLOAD RECEIVED]: '%s'(%lu)\n", request.payload, strlen(request.payload));

    if(strcmp(request.payload, "##kill") == 0)  tcp_server_t_terminate(request.server);
    else if(request.payload[0] == '+')          register_to_channel(request.payload, request.payload_length, reply.client_id);
    else if(request.payload[0] == '-')          unregister_from_channel(request.payload, request.payload_length, reply.client_id);
    else                                        broadcast_in_channel(request.payload, request.payload_length, reply.client_id);
}

int main(int argc, char *argv[]) {
    LOG("Starting Publish/Subscribe Server!\n");

    if(argc < 2) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);

    channel_table_t_create(&channels);


    tcp_server_t_create(&server, TCP_SERVER_SYNC);
    tcp_server_t_bind_to_port(server, port);
    tcp_server_t_set_request_handler(server, server_handler);
    tcp_server_t_start(server);

    LOG("Terminating Publish/Subscribe Server!\n");

    channel_table_t_destroy(channels);
    tcp_server_t_destroy(server);
}