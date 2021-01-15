#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#include "utils.h"

#define TCP_SERVER_MAX_PAYLOAD_LENGTH 500
#include "network/tcp_server.h"

#include "channel_table.h"
#include "burned_table.h"

struct tcp_server_t* server = NULL;
struct channel_table_t* channels = NULL;

void register_to_channel(const char* payload, unsigned int payload_length, int client)
{
    char reply_message[500] = {'\0'};
    char channel[500] = {'\0'};

    memcpy(channel, &payload[1], payload_length - 2);

    if(channel_table_t_get_client(channels, channel, client) != NULL)
    {
        sprintf(reply_message,"already subscribed +%s\n", channel);
        send_message_to_client(client, reply_message);
        return;
    }
    printf("asdasd\n");
    channel_table_t_add(channels, channel, client);
    sprintf(reply_message,"subscribed +%s\n", channel);

    send_message_to_client(client, reply_message);
}

void unregister_from_channel(const char* payload, unsigned int payload_length, int client)
{
    char reply_message[500];

    char channel[500] = {'\0'};
    memcpy(channel, &payload[1], payload_length - 2);

    struct client_node_t * cli = channel_table_t_get_client(channels, channel, client);

    if(cli == NULL)
    {
        sprintf(reply_message,"not subscribed -%s\n", channel);
        send_message_to_client(client, reply_message);
        return;
    }

    channel_table_t_remove(channels, channel, client);

    sprintf(reply_message, "unsubscribed -%s\n", channel);
    send_message_to_client(client, reply_message);
}


void broadcast_in_channel(const char* payload, unsigned payload_len, int client)
{
    struct burned_table_t* table = NULL;
    char reply_message[500];

    char message[TCP_SERVER_MAX_PAYLOAD_LENGTH] = {'\0'};
    
    memcpy(message, payload, payload_len);

    char** tags = NULL;
    unsigned long n_tags = 0;

    if(parse_message(message, strlen(message), &tags, &n_tags) < 0)
    {
        return;
    }

    for(int i=0; i<n_tags; i++)
    {
        burned_table_t_create(&table);

        unsigned long message_length = payload_len;
        
        struct client_node_t* client = channel_table_t_get_channel(channels, tags[i]);
        
        while(client) {
            if(burned_table_t_get_client(table, client->client_id))
                continue;
            
            burned_table_t_add(table, client->client_id);

            send_message_to_client(client->client_id, message);
            
            client = client->prev;
        }

        burned_table_t_destroy(&table);

    }
}

void server_handler(struct request_t request, struct reply_t reply)
{
    LOG("Payload: %s!\n", request.payload);

    LOG("Verificando payload!\n");
    unsigned long m_size = strlen(request.payload);

    for(int i=0; i<m_size; i++) {
        LOG("%c = %i\n", request.payload[i], request.payload[i]);
        if(!verify_char(request.payload[i]))
        {
            tcp_server_t_disconnect_client(request.server, reply.client_id);
            return;
        }
    }

    LOG("[PAYLOAD RECEBIDO]: tamanho=%lu\n", strlen(request.payload));

    if(strcmp(request.payload, "##kill\n") == 0)  tcp_server_t_terminate(request.server);
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

    tcp_server_t_set_disconnect_message(server, "##kill");
    tcp_server_t_set_request_handler(server, server_handler);

    tcp_server_t_bind_to_port(server, port);
    tcp_server_t_start(server);

    LOG("Terminating Publish/Subscribe Server!\n");
    channel_table_t_destroy(channels);
    LOG("Terminating Publish/Subscribe Server!\n");
    tcp_server_t_destroy(server);
    LOG("Terminating Publish/Subscribe Server!\n");
}