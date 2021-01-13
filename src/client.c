#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 

#include "network/async.h"
#include "network/client.h"

#include "terminal.h"

struct client_t* client = NULL;
// struct thread_t* sender_thread = NULL;
int should_stop = 0;


// void on_receive(struct client_t* client, char* message)
// {
//     if(strcmp(message, "##kill") == 0)
//     {
//         should_stop = 1;
//         return;
//     }

//     if(strlen(message))
//         printf("%s\n", message);
// }


int main(int argc, char *argv[]) {
    if(argc < 3) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    const char* url = argv[1];
    int port = atoi(argv[2]);

    client_t_create(&client, url, port);

    char message[500] = {'\0'}, received[500] = {'\0'}, cpy[500] = {'\0'};

    int stop = 0;

    while(
        strcmp(message, "##kill") != 0
    ) {
        stop = 0;
        while(!keyboard_input())
        {
            bzero(received, 500);
            int rec = client_t_receive(client, received, 500);

            if(rec != -1)
            {
                if(strcmp(received, "##kill") == 0)
                {
                    stop = 1;
                    break;
                }
                printf("%s\n", received);
            }
        }
    
        if(stop) break;

        bzero(cpy, 500);
        bzero(message, 500);

        if(read_inputed_line(message, 500) == -1)
        {
            printf("Não é possivel enviar mensagens maiores que 500 caracteres!");
            continue;
        }

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
            client_t_send(client, "/channel/mute/", cpy);
        }
        else if(strcmp(message, "##kill") == 0)
        {
            bzero(cpy, 500);
            memcpy(cpy, &message[1], 500);
            client_t_send(client, "/kill/", cpy);
        }
        else
        {
            bzero(cpy, 500);
            memcpy(cpy, &message[0], 500);
            client_t_send(client, "/broadcast/", cpy);
        }
    }

    client_t_disconnect(client);
    client_t_destroy(client);
}