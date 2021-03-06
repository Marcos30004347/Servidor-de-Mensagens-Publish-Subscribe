#include "tcp_client.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#include <sys/select.h>
#include <fcntl.h>

struct tcp_client_t;

typedef void(*client_handler)(struct tcp_client_t*, char*);

struct tcp_client_t
{
    struct sockaddr_in server_adress;
    int client_fd;
	client_handler on_receive;
};


int tcp_client_t_receive(struct tcp_client_t* client, char* message, int length)
{
	int sd = client->client_fd;

	fd_set input;
	FD_ZERO(&input);
	FD_SET(sd, &input);

	struct timeval timeout = {0, 0};

	int n = select(sd + 1, &input, NULL, NULL, &timeout);

	if (n <= 0 || !FD_ISSET(sd, &input)) {
		return -1;
	}

	read(client->client_fd, message, sizeof(char)*length);
	return 1;
}

void tcp_client_t_send(struct tcp_client_t* client, char* message)
{
	if(strlen(message) > TCP_CLIENT_MAX_PAYLOAD_LENGTH)
		return;

	write(client->client_fd, message, strlen(message)); 
}


void tcp_client_t_create(struct tcp_client_t** client, const char* url, int port)
{
    *client = (struct tcp_client_t*)malloc(sizeof(struct tcp_client_t));
    struct tcp_client_t* c = *client;

    c->client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (c->client_fd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	}

    c->server_adress.sin_family = AF_INET; 
	c->server_adress.sin_addr.s_addr = inet_addr(url); 
	c->server_adress.sin_port = htons(port); 

	int conn = connect(c->client_fd, (struct sockaddr *)&c->server_adress, sizeof(c->server_adress));
	if (conn != 0){
		printf("connection with the server failed with %i...\n", conn); 
		exit(0); 
	}

	int flags = fcntl(c->client_fd, F_GETFL);
	fcntl(c->client_fd, F_SETFL ,flags | O_NONBLOCK);
}

void tcp_client_t_destroy(struct tcp_client_t* client)
{
	free(client);
}

void tcp_client_t_disconnect(struct tcp_client_t* client)
{
	close(client->client_fd);
}