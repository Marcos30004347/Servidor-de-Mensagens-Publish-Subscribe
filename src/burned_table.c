#include "burned_table.h"

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "hash.h"


void burned_table_t_create(struct burned_table_t** table)
{
    *table = (struct burned_table_t*)malloc(sizeof(struct burned_table_t));
    mutex_t_create(&(*table)->lock);
    for(int i=0; i<128; i++)
    {
        (*table)->table[i] = NULL;
    }
}

void burned_table_t_add(struct burned_table_t* table, int client) {
    mutex_t_lock(table->lock);

    // If no channel with 'string' name exist yet, it should be created
    if(!table->table[client%128]) {
        table->table[client%128] = (struct burned_node_t*)malloc(sizeof(struct burned_node_t));
        table->table[client%128]->next = NULL;
        table->table[client%128]->key = client;
        return;
    }

    struct burned_node_t** tail = &table->table[client%128];

    struct burned_node_t* cli = (struct burned_node_t*)malloc(sizeof(struct burned_node_t));

    cli->next = NULL;
    cli->key = client;

    while((*tail)->next && (*tail)->key != client)
        tail = &(*tail)->next;

    if((*tail)->key == client) return;

    (*tail)->next = cli;
    mutex_t_unlock(table->lock);
}

int burned_table_t_get_client(struct burned_table_t* table, int client)
{
    if(!table->table[client%128])
        return 0;

    struct burned_node_t* tail = table->table[client%128];

    while(tail->next && tail->key != client)
        tail = tail->next;

    if(tail->key == client) return 1;

    return 0;
}



void burned_table_t_destroy(struct burned_table_t* table)
{
    for(int i=0; i<128; i++)
    {
        while (table->table[i])
        {
            struct burned_node_t* tmp = table->table[i];
            table->table[i] = table->table[i]->next;
            free(tmp);
        }
    }

    mutex_t_destroy(table->lock);
}