all: server client

server:
	gcc \
	src/hash.c \
	src/server.c \
	src/channel_table.c \
	src/burned_table.c \
	src/network/tcp_server.c \
	src/network/async.c \
	-o servidor -lpthread

client:
	gcc \
	src/client.c \
	src/network/tcp_client.c \
	src/network/async.c \
	-o cliente -lpthread