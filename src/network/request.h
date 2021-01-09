#ifndef REQUEST_H
#define REQUEST_H

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>


struct server_t;


struct request_t
{
    char* endpoint;
    const char* message;
    struct server_t* server;
};

struct reply_t
{
    int client;
};

typedef void (*server_handler)(struct request_t req, struct reply_t reply);


void reply_t_send(int client, const char* message);

#endif