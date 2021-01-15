#ifndef burned_H
#define burned_H

#include "network/async.h"


/**
 * This structure holds information about a registered 
 * burned and all clients interested on it.
 */
struct burned_node_t
{
    int key;
    struct burned_node_t* next;
};

struct burned_table_t
{
    struct burned_node_t* table[128];
};


void burned_table_t_create(struct burned_table_t** table);
void burned_table_t_add(struct burned_table_t* table, int client);
void burned_table_t_destroy(struct burned_table_t** table);

int burned_table_t_get_client(struct burned_table_t* table, int cid);

#endif