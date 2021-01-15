#ifndef tag_H
#define tag_H

#include "network/async.h"

/**
 * @brief A clinet linked list.
 */
struct client_node_t {
    int client_id;
    struct client_node_t* next;
    struct client_node_t* prev;
};

/**
 * @brief A tag that holds all the registered
 * clients.
 */
struct tag_node_t
{
    char* tag_name;
    struct tag_node_t* next;
    struct client_node_t* clients;
};

/**
 * @brief A tag table that holds all the
 * registered tags and clients.
 */
struct tag_table_t
{
    struct mutex_t* lock;
    struct tag_node_t* table[128];
};


/**
 * @brief Creates a tag table object.
 * 
 * @param table The tag table object.
 */
void tag_table_t_create(struct tag_table_t** table);

/**
 * @brief Add a client to the table[tag] spot.
 * 
 * @param table The table object.
 * @param tag The tag name. 
 * @param client The client that should be added.
 */
void tag_table_t_add(struct tag_table_t* table, const char* tag, int client);

/**
 * @brief Remove a client from the table[tag] spot.
 * 
 * @param table The table object.
 * @param tag The tag name. 
 * @param client The client that should be removed.
 */
void tag_table_t_remove(struct tag_table_t* table, const char* tag, int client);

/**
 * @brief Destroys a tag table object.
 * 
 * @param table 
 */
void tag_table_t_destroy(struct tag_table_t* table);

/**
 * @brief Get a tag client list from the table.
 * 
 * @param table The table object.
 * @param tag The tag name.
 * @return The linked list that holds all the clients registered in that tag.
 */
struct client_node_t* tag_table_t_get_tag(struct tag_table_t* table, const char* tag);

/**
 * @brief Return a client linked list that starts at the client node with the specified cid.
 * 
 * @param table The table object.
 * @param tag The tag name.
 * @param cid The id of the client. 
 * @return The linked list.
 */
struct client_node_t* tag_table_t_get_client(struct tag_table_t* table, const char* tag, int cid);

#endif