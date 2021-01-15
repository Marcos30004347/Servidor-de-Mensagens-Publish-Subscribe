#include "tag_table.h"

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "hash.h"


void tag_table_t_create(struct tag_table_t** table)
{
    *table = (struct tag_table_t*)malloc(sizeof(struct tag_table_t));
    mutex_t_create(&(*table)->lock);
    for(int i=0; i<128; i++)
    {
        (*table)->table[i] = NULL;
    }
}

void tag_table_t_add(struct tag_table_t* table, const char* string, int client) {
    mutex_t_lock(table->lock);
    int id = hash_string(string)%128;

    // If no channel with 'string' name exist yet, it should be created
    if(!table->table[id]) {
        table->table[id] = (struct tag_node_t*)malloc(sizeof(struct tag_node_t));
        table->table[id]->tag_name = (char*)malloc(sizeof(char)*strlen(string));
        memcpy(table->table[id]->tag_name, string, sizeof(char)*strlen(string));
        table->table[id]->clients = NULL;
    }

    struct tag_node_t** tail = &table->table[id];

    while(strcmp((*tail)->tag_name, string) == 0 && (*tail)->next) {
        (*tail) = (*tail)->next;
    }

    // In case of collisions with a previous registered channel
    if(strcmp((*tail)->tag_name, string) != 0) {
        (*tail)->next = (struct tag_node_t*)malloc(sizeof(struct tag_node_t));
        (*tail)->next->tag_name = (char*)malloc(sizeof(char)*strlen(string));
        memcpy((*tail)->next->tag_name, string, sizeof(char)*strlen(string));

        (*tail)->next->next = NULL;
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
    mutex_t_unlock(table->lock);

}

struct client_node_t* tag_table_t_get_client(struct tag_table_t* table, const char* string, int cid)
{
    int id = hash_string(string)%128;
    if(!table->table[id])
    {
        return NULL;
    }

    struct tag_node_t* tail = table->table[id];
    while(strcmp(tail->tag_name, string) == 0 && tail->next) {
        tail = tail->next;
    }

    if(strcmp(tail->tag_name, string) != 0) return NULL;

    struct client_node_t* client = tail->clients;

    while(client) {
        if(client->client_id == cid) {
            return client;
        }
        client = client->prev;
    }
    return NULL;
}

void tag_table_t_remove(struct tag_table_t* table, const char* string, int client) {
    mutex_t_lock(table->lock);

    int id = hash_string(string)%128;

    struct client_node_t* cli = tag_table_t_get_client(table, string, client);

    if(!cli)
        return;

    if(cli->prev)
        cli->prev->next = cli->next;
    if(cli->next)
        cli->next->prev = cli->prev;


    if(table->table[id]->clients == cli) {
        free(table->table[id]->tag_name);
        table->table[id]->clients = NULL;
    }

    free(cli);
    mutex_t_unlock(table->lock);
}

struct client_node_t* tag_table_t_get_tag(struct tag_table_t* table, const char* string)
{
    int id = hash_string(string)%128;
    if(!table->table[id])
    {
        return NULL;
    }
    struct tag_node_t* tail = table->table[id];
    while(strcmp(tail->tag_name, string) == 0 && tail->next) {
        tail = tail->next;
    }
    if(strcmp(tail->tag_name, string) != 0) return NULL;


    return tail->clients;
}


void tag_table_t_destroy(struct tag_table_t* table)
{
    for(int i=0; i<128; i++)
    {
        while (table->table[i])
        {
            struct tag_node_t* tmp = table->table[i];

            while(table->table[i]->clients) {
                if(table->table[i]->clients->prev) table->table[i]->clients->prev->next = table->table[i]->clients->next;
                if(table->table[i]->clients->next) table->table[i]->clients->next->prev = table->table[i]->clients->prev;
                struct client_node_t* tmp = table->table[i]->clients;
                table->table[i]->clients = table->table[i]->clients->prev;
                free(tmp);
            }

            table->table[i] = table->table[i]->next;
            free(tmp->tag_name);
            free(tmp);
        }
    }
    mutex_t_destroy(table->lock);
    free(table);
}