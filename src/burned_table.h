#ifndef burned_H
#define burned_H

#include "network/async.h"


/**
 * @brief  This structure holds a client that
 * have already received a message.
 */
struct burned_node_t
{
    int key;
    struct burned_node_t* next;
};

/**
 * @brief  This structure holds al clients that
 * have already received a message.
 */
struct burned_table_t
{
    struct burned_node_t* table[128];
};

/**
 * @brief Creates a burned table object.
 * 
 * @param table The table object.
 */
void burned_table_t_create(struct burned_table_t** table);

/**
 * @brief Add a client to the table
 * 
 * @param table The table object.
 * @param client The client id.
 */
void burned_table_t_add(struct burned_table_t* table, int client);

/**
 * @brief Destroys aburned table object.
 * 
 * @param table The table object.
 */
void burned_table_t_destroy(struct burned_table_t** table);

/**
 * @brief Get if a client is inserted on the table.
 * 
 * @param table The table object.
 * @param cid The of the client.
 * @return 1 if the client is on the table and 0 otherwise. 
 */
int burned_table_t_get_client(struct burned_table_t* table, int cid);

#endif