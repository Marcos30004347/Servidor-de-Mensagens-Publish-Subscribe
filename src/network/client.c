#include "client.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#include "async.h"

struct client_t;

typedef void(*client_handler)(struct client_t*, char*);

struct client_t
{
    struct sockaddr_in server_adress;

	struct thread_t* receiver;
	struct mutex_t* lock;

    int client_fd;

	client_handler on_receive;
};


void* reader_therad(void* args)
{
	struct client_t* client = (struct client_t*)(args);
	char message[500];

	while(1) {
		bzero(message, 500);
		read(client->client_fd, message, sizeof(char)*500);

		if(client->on_receive)
			client->on_receive(client, message);
	}

	return NULL;
}

void client_t_send(struct client_t* client, const char* url, char* message) {
	char result[500];
	// bzero(result, 5000);
	result[0] = '\0';

	strcat(result, url);
	strcat(result, "\r\n");
	strcat(result, message);
	strcat(result, "\r\n");
	// printf("sending %s\n", result);

	write(client->client_fd, result, sizeof(result)); 
}


void client_t_create(struct client_t** client, client_handler handler, const char* url, int port)
{
    *client = (struct client_t*)malloc(sizeof(struct client_t));
    struct client_t* c = *client;

    c->client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (c->client_fd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	}

    c->server_adress.sin_family = AF_INET; 
	c->server_adress.sin_addr.s_addr = inet_addr(url); 
	c->server_adress.sin_port = htons(port); 
	c->on_receive = handler;
	if (connect(c->client_fd, (struct sockaddr *)&c->server_adress, sizeof(c->server_adress)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 

	mutex_t_create(&c->lock);	
	thread_t_create(&c->receiver, reader_therad, c);

}


// void client_t_terminate(struct client_t* client)
// {
// 	thread_t_cancel(client->receiver);
// 	thread_t_join(client->receiver);
// }

void client_t_destroy(struct client_t* client)
{
	close(client->client_fd);
	thread_t_destroy(client->receiver);
	mutex_t_destroy(client->lock);
	free(client);
}