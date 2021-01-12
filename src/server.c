#include "network/server.h"
#include "network/request.h"

#include "channel_table.h"

struct server_t* server = NULL;
struct channel_table_t* channels = NULL;

void add_channel(struct request_t request, struct reply_t reply)
{
    char reply_message[500];
    bzero(reply_message, sizeof(reply_message));

    char* channel = (char*)request.message;
    
    if(channel_table_t_get_client(channels, channel, reply.client) != NULL)
    {
        sprintf(reply_message,"already subscribed +%s", channel);
        reply_t_send(reply.client, reply_message);
    }
    else 
    {
        channel_table_t_add(channels, channel, reply.client);
        sprintf(reply_message,"subscribed  +%s", channel);
        reply_t_send(reply.client, reply_message);
    }

}

void remove_channel(struct request_t request, struct reply_t reply)
{
    char reply_message[500];
    char* channel = (char*)request.message;

    if(channel_table_t_get_client(channels, channel, reply.client) == NULL)
    {
        sprintf(reply_message,"not subscribed -%s", channel);
        reply_t_send(reply.client, reply_message);
    }
    else
    {
        channel_table_t_remove(channels, channel, reply.client);
        sprintf(reply_message,"unsubscribed -%s", channel);
        reply_t_send(reply.client, reply_message);
    }
}


void broadcast(struct request_t request, struct reply_t reply)
{
    char* message = (char*)request.message;
 
    char* channel = NULL;
    char* text = NULL;

    text = strtok(message, "#");
    channel = strtok(NULL, "#");
    while(channel != NULL) {

        struct client_node_t* client = channel_table_t_get_channel(channels, channel);

        while(client) {
            reply_t_send(client->client_id, text);
            client = client->prev;
        }

        channel = strtok(NULL, "#");
    }
}

void kill(struct request_t request, struct reply_t reply)
{
    server_t_terminate(request.server);
    return;
}

void disconnect(struct request_t request, struct reply_t reply)
{
    server_t_disconnect_client(request.server, reply.client);
    return;
}

int main(int argc, char *argv[]) {
    if(argc < 2) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);

    channel_table_t_create(&channels);

    server_t_create(&server);
    server_t_bind_to_port(server, port);

    server_t_add_endpoint(server, "/channel/listen/", add_channel);
    server_t_add_endpoint(server, "/channel/mute/", remove_channel);
    server_t_add_endpoint(server, "/broadcast/", broadcast);
    server_t_add_endpoint(server, "/disconnect/", disconnect);
    server_t_add_endpoint(server, "/kill/", kill);

    server_t_start(server);

    channel_table_t_destroy(channels);
    server_t_destroy(server);
}