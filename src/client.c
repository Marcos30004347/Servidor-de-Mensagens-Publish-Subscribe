#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 

#include "network/async.h"
#include "network/client.h"

struct client_t* client = NULL;
struct thread_t* sender_thread = NULL;

void on_receive(struct client_t* client, char* message)
{
    if(strlen(message))
        printf("%s\n", message);

    if(strcmp(message, "##kill") == 0)
    {
        printf("exiting\n");
        thread_t_cancel(sender_thread);
        // client_t_terminate(client);
    }
}

void* sender(void* args)
{
    struct client_t* client = (struct client_t*)args;
    char message[501];
	char cpy[500];

    while(1)
    {
        bzero(cpy, 500);
        bzero(message, 500);
        fgets(message, 500, stdin);

        if(message[0] == '+')
        {
            bzero(cpy, 500);
            memcpy(cpy, &message[1], 500);
            client_t_send(client, "/channel/listen/", cpy);
        }
        else if(message[0] == '-')
        {
            bzero(cpy, 500);
            memcpy(cpy, &message[1], 500);
            client_t_send(client, "/channel/mute/", cpy);		}
        else
        {
            bzero(cpy, 500);
            memcpy(cpy, &message[0], 500);
            client_t_send(client, "/broadcast/", cpy);
        }
    }

    return NULL;
}
int main() {
    client_t_create(&client, on_receive, "127.0.0.1", 4);

    thread_t_create(&sender_thread, sender, client);
    thread_t_join(sender_thread);

    client_t_destroy(client);
}