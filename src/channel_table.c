#include "channel_table.h"

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "network/hash.h"

void client_node_t_remove(struct client_node_t** node)
{
    if((*node)->prev) (*node)->prev->next = (*node)->next;
    if((*node)->next) (*node)->next->prev = (*node)->prev;

    free((*node));

    *node = NULL;
    node = NULL;
}

void channel_table_t_create(struct channel_table_t** table)
{
    *table = (struct channel_table_t*)malloc(sizeof(struct channel_table_t));
    for(int i=0; i<128; i++)
    {
        (*table)->table[i] = NULL;
    }
}

void channel_table_t_add(struct channel_table_t* table, const char* string, int client) {
    int id = hash_string(string)%128;

    // If no channel with 'string' name exist yet, it should be created
    if(!table->table[id]) {
        table->table[id] = (struct channel_node_t*)malloc(sizeof(struct channel_node_t));
        table->table[id]->channel_name = (char*)malloc(sizeof(char)*strlen(string));
        memcpy(table->table[id]->channel_name, string, sizeof(char)*strlen(string));
        table->table[id]->clients = NULL;
    }

    struct channel_node_t** tail = &table->table[id];

    while(strcmp((*tail)->channel_name, string) == 0 && (*tail)->next) {
        (*tail) = (*tail)->next;
    }

    // In case of collisions with a previous registered channel
    if(strcmp((*tail)->channel_name, string) != 0) {
        (*tail)->next = (struct channel_node_t*)malloc(sizeof(struct channel_node_t));
        (*tail)->next->channel_name = (char*)malloc(sizeof(char)*strlen(string));
        memcpy(table->table[id]->channel_name, string, sizeof(char)*strlen(string));

        (*tail)->next->next = NULL;
        table->table[id]->clients = NULL;
        (*tail) = (*tail)->next;
    }

    struct client_node_t* cli = (struct client_node_t*)malloc(sizeof(struct client_node_t));
    cli->next = NULL;
    cli->prev = NULL;
    cli->client_id = client;

    if(!(*tail)->clients)
        (*tail)->clients = cli;
    else
    {
        (*tail)->clients->next = cli;
        cli->prev = (*tail)->clients;
        (*tail)->clients = cli;
    }
}

void channel_table_t_remove(struct channel_table_t* table, const char* string, int client) {
    int id = hash_string(string)%128;

    if(!table->table[id]) return;

    struct channel_node_t** tail = &table->table[id];
    while(strcmp((*tail)->channel_name, string) != 0 && (*tail)->next) {
        (*tail) = (*tail)->next;
    }

    if(strcmp((*tail)->channel_name, string) != 0) return;
    struct client_node_t* cli = (*tail)->clients;
     
    while(cli && cli->client_id != client) {
        cli = cli->prev;
    }

    if(cli) {
        if(cli->prev)
            cli->prev->next = cli->next;
        if(cli->next)
            cli->next->prev = cli->prev;

        free(cli);
    }

    if(!(*tail)->clients) {
        free((*tail)->channel_name);

        (*tail) = NULL;
        tail = NULL;
    }
}

struct client_node_t* channel_table_t_get_channel(struct channel_table_t* table, const char* string)
{
    int id = hash_string(string)%128;
    if(!table->table[id])
    {
        return NULL;
    }
    struct channel_node_t* tail = table->table[id];
    while(strcmp(tail->channel_name, string) == 0 && tail->next) {
        tail = tail->next;
    }
    if(strcmp(tail->channel_name, string) != 0) return NULL;


    return tail->clients;
}

struct client_node_t* channel_table_t_get_client(struct channel_table_t* table, const char* string, int cid)
{

    int id = hash_string(string)%128;
    if(!table->table[id])
    {
        return NULL;
    }

    struct channel_node_t* tail = table->table[id];
    while(strcmp(tail->channel_name, string) == 0 && tail->next) {
        tail = tail->next;
    }
    if(strcmp(tail->channel_name, string) != 0) return NULL;


    return tail->clients;
    struct client_node_t* client = tail->clients;

    while(client) {
        if(client->client_id == cid) {
            return client;
        }
        client = client->prev;
    }
    return NULL;
}


void channel_table_t_destroy(struct channel_table_t* table)
{
    for(int i=0; i<128; i++)
    {
        while (table->table[i])
        {
            struct channel_node_t* tmp = table->table[i];
            table->table[i] = table->table[i]->next;

            while(table->table[i]->clients) {
                client_node_t_remove(&table->table[i]->clients);
            }
            free(tmp->channel_name);
            free(tmp);
        }
    }
}