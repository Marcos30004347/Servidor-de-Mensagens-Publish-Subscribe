// STD
#include<string.h>
#include<stdlib.h>

// UTILS
#define DEBUG
#include "utils.h"

// NETWORK TCP
#define TCP_SERVER_MAX_PAYLOAD_LENGTH 500
#include "network/tcp_server.h"

// PROTOCOL
#include "protocol/parser.h"

// APPLICATION
#include "tag_table.h"
#include "burned_table.h"


/**
 * @brief The TCP Server of the application.
 */
struct tcp_server_t* server = NULL;


/**
 * @brief Table responsable for holding all the registered tags and clients.
 */
struct tag_table_t* channels = NULL;


/**
 * @brief This function add a client to a tag
 */
void register_to_tag(const char* payload, unsigned int payload_length, int client, protocol_ast_t* ast)
{
    char reply_message[500] = {'\0'};
    protocol_ast_t* channel = ast;
    if(tag_table_t_get_client(channels, channel->ast_node_add_value, client) != NULL)
    {
        sprintf(reply_message,"already subscribed +%s\n", channel->ast_node_add_value);
        send_message_to_client(client, reply_message);
        return;
    }

    tag_table_t_add(channels, channel->ast_node_add_value, client);
    sprintf(reply_message,"subscribed +%s\n", channel->ast_node_add_value);

    send_message_to_client(client, reply_message);
}

/**
 * @brief This function removes a client from a tag
 */
void unregister_from_tag(const char* payload, unsigned int payload_length, int client, protocol_ast_t* ast)
{
    char reply_message[500];
    protocol_ast_t* channel = ast;

    struct client_node_t * cli = tag_table_t_get_client(channels, channel->ast_node_rem_value, client);

    if(cli == NULL)
    {
        sprintf(reply_message,"not subscribed -%s\n", channel->ast_node_rem_value);
        send_message_to_client(client, reply_message);
        return;
    }

    tag_table_t_remove(channels, channel->ast_node_rem_value, client);

    sprintf(reply_message, "unsubscribed -%s\n", channel->ast_node_rem_value);
    send_message_to_client(client, reply_message);
}

/**
 * @brief This function broadcast a message to some tags.
 */
void broadcast_in_channel(const char* payload, unsigned payload_len, int client, protocol_ast_t* ast)
{
    struct burned_table_t* table = NULL;
    char reply_message[500];
    burned_table_t_create(&table);

    while(ast)
    {
        if(ast->ast_node_type == PROTOCOL_AST_TAG)
        {
            unsigned long message_length = payload_len;
            struct client_node_t* client = tag_table_t_get_tag(channels, ast->ast_node_tag_value);
            while(client) {
                if(burned_table_t_get_client(table, client->client_id))
                    continue;
                
                burned_table_t_add(table, client->client_id);

                send_message_to_client(client->client_id, payload);
                
                client = client->prev;
            }

        }
        ast = ast->ast_node_child;
    }

    burned_table_t_destroy(&table);
}

/**
 * @brief This function is responsable for handling the server requests.
 */
void server_handler(struct request_t request, struct reply_t reply)
{
    LOG("[PAYLOAD]: '%s'", request.payload);
    if(strcmp(request.payload, "##kill\n") == 0)    tcp_server_t_terminate(request.server);

    unsigned long m_size = strlen(request.payload);

    protocol_lexer_t* lexer = NULL;
    protocol_ast_t* abstract_syntax_tree = NULL;

    protocol_lexer_t_create(&lexer, request.payload, request.payload_length);

    int error = protocol_ast_t_parse(lexer, &abstract_syntax_tree);

    LOG("[AST]: '");
    protocol_ast_t_print(abstract_syntax_tree);
    printf("'\n");

    if(error)
    {
        printf("Syntax Error!\n");
        protocol_lexer_t_destroy(&lexer);
        protocol_ast_t_destroy(&abstract_syntax_tree);
        tcp_server_t_disconnect_client(request.server, reply.client_id);
        return;
    }

    if(!abstract_syntax_tree->ast_node_child) {
        printf("Empty Payload!\n");
        return;
    }

    if(abstract_syntax_tree->ast_node_child->ast_node_type == PROTOCOL_AST_ADD)
        register_to_tag(
            request.payload,
            request.payload_length,
            reply.client_id,
            abstract_syntax_tree->ast_node_child
        );
    else if(abstract_syntax_tree->ast_node_child->ast_node_type == PROTOCOL_AST_REM)
        unregister_from_tag(
            request.payload,
            request.payload_length,
            reply.client_id,
            abstract_syntax_tree->ast_node_child
        );
    else if(
        abstract_syntax_tree->ast_node_child->ast_node_type == PROTOCOL_AST_MESS
        || abstract_syntax_tree->ast_node_child->ast_node_type == PROTOCOL_AST_TAG
    )
        broadcast_in_channel(
            request.payload,
            request.payload_length,
            reply.client_id,
            abstract_syntax_tree->ast_node_child
        );
    else    
        printf(
            "Unknow Operation of type '%i' in '%s'!\n",
            abstract_syntax_tree->ast_node_child->ast_node_type,
            request.payload
        );


    protocol_lexer_t_destroy(&lexer);
    protocol_ast_t_destroy(&abstract_syntax_tree);
}

int main(int argc, char *argv[]) {
    // LOG("Starting Publish/Subscribe Server!\n");

    if(argc < 2) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);

    tag_table_t_create(&channels);

    tcp_server_t_create(&server, TCP_SERVER_SYNC);
    tcp_server_t_set_disconnect_message(server, "##kill");
    tcp_server_t_set_request_handler(server, server_handler);
    tcp_server_t_bind_to_port(server, port);
    tcp_server_t_start(server);

    tag_table_t_destroy(channels);
    tcp_server_t_destroy(server);
}