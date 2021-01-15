#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 

#include "network/async.h"

#define TCP_CLIENT_MAX_PAYLOAD_LENGTH 500
#include "network/tcp_client.h"
#include "protocol/parser.h"

#include "terminal.h"
#include "utils.h"


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

    char message [TCP_CLIENT_MAX_PAYLOAD_LENGTH] = {'\0'};
    char received[TCP_CLIENT_MAX_PAYLOAD_LENGTH] = {'\0'};

    int stop_client = 0;

    while(strcmp(message, "##kill") != 0)
    {
        stop_client = 0;
        while(!keyboard_input())
        {
            bzero(received, TCP_CLIENT_MAX_PAYLOAD_LENGTH);
            if(tcp_client_t_receive(client, received, TCP_CLIENT_MAX_PAYLOAD_LENGTH) != -1)
            {
                if(strcmp(received, "##kill") == 0)
                {
                    stop_client = 1;
                    break;
                }
    
                unsigned long m_size = strlen(received);
    
                protocol_lexer_t* lexer = NULL;
                protocol_ast_t* abstract_syntax_tree = NULL;
            
                protocol_lexer_t_create(&lexer, received, m_size);
            
                int error = protocol_ast_t_parse(lexer, &abstract_syntax_tree);
                protocol_lexer_t_destroy(&lexer);
                protocol_ast_t_destroy(&abstract_syntax_tree);
        
                if(error)
                {
                    continue;
                }

                printf("%s", received);
            }
        }
    
        if(stop_client) break;

        bzero(message, TCP_CLIENT_MAX_PAYLOAD_LENGTH);

        if(read_inputed_line(message, TCP_CLIENT_MAX_PAYLOAD_LENGTH) == -1)
            continue;

        protocol_lexer_t* lexer = NULL;
        protocol_ast_t* abstract_syntax_tree = NULL;
    
        protocol_lexer_t_create(&lexer, message, strlen(message));
    
        int error = protocol_ast_t_parse(lexer, &abstract_syntax_tree);
        protocol_lexer_t_destroy(&lexer);
        protocol_ast_t_destroy(&abstract_syntax_tree);
        
        if(error)
        {
            continue;
        }

        tcp_client_t_send(client, message);
    }

    tcp_client_t_disconnect(client);
    tcp_client_t_destroy(client);
}