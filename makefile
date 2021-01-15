all: server client

protocol-tester:
	gcc \
	src/protocol/main.c \
	src/protocol/lexer.c \
	src/protocol/parser.c \
	src/protocol/token.c \
	-o protocol

server:
	gcc \
	src/hash.c \
	src/server.c \
	src/protocol/parser.c \
	src/protocol/lexer.c \
	src/protocol/token.c \
	src/tag_table.c \
	src/burned_table.c \
	src/network/tcp_server.c \
	src/network/async.c \
	-o servidor -lpthread

client:
	gcc \
	src/client.c \
	src/terminal.c \
	src/network/tcp_client.c \
	src/network/async.c \
	src/protocol/parser.c \
	src/protocol/lexer.c \
	src/protocol/token.c \
	-o cliente -lpthread