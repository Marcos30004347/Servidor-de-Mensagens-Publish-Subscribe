all: server client

server:
	gcc \
	src/hash.c \
	src/server.c \
	src/channel_table.c \
	src/network/server.c \
	src/network/async.c \
	-o server.out -lpthread

client:
	gcc \
	src/client.c \
	src/network/client.c \
	src/network/async.c \
	-o client.out -lpthread