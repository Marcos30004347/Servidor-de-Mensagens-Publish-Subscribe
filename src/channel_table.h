#ifndef CHANNEL_H
#define CHANNEL_H

#include "network/async.h"

struct client_node_t {
    int client_id;
    struct client_node_t* next;
    struct client_node_t* prev;
};

struct channel_node_t
{
    char* channel_name;
    struct channel_node_t* next;
    struct client_node_t* clients;
};

struct channel_table_t
{
    struct mutex_t* lock;
    struct channel_node_t* table[128];
};


void channel_table_t_create(struct channel_table_t** table);
void channel_table_t_add(struct channel_table_t* table, const char* string, int client);
void channel_table_t_remove(struct channel_table_t* table, const char* string, int client);
void channel_table_t_destroy(struct channel_table_t* table);

struct client_node_t* channel_table_t_get_channel(struct channel_table_t* table, const char* string);
struct client_node_t* channel_table_t_get_client(struct channel_table_t* table, const char* string, int cid);

#endif