#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 

#include "network/async.h"
#include "network/tcp_client.h"

#include "terminal.h"


int main(int argc, char *argv[]) {
    if(argc < 3) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    const char* url = argv[1];
    int port = atoi(argv[2]);

    struct tcp_client_t* client = NULL;
    tcp_client_t_create(&client, url, port);

    char message[500] = {'\0'}, received[500] = {'\0'};

    int stop_client = 0;

    while(strcmp(message, "##kill") != 0)
    {
        stop_client = 0;
        while(!keyboard_input())
        {
            bzero(received, 500);
            if(tcp_client_t_receive(client, received, 500) != -1)
            {
                if(strcmp(received, "##kill") == 0)
                {
                    stop_client = 1;
                    break;
                }
                printf("%s\n", received);
            }
        }
    
        if(stop_client) break;

        bzero(message, 500);

        if(read_inputed_line(message, 500) == -1)
        {
            printf("Não é possivel enviar mensagens maiores que 500 caracteres!");
            continue;
        }

        tcp_client_t_send(client, message);
    }

    tcp_client_t_disconnect(client);
    tcp_client_t_destroy(client);
}