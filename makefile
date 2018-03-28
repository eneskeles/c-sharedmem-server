all: server test_client

server: server.c
	gcc -Wall -g -o server server.c -lrt -pthread

test_client: test_client.c
	gcc -Wall -g -o test_client test_client.c -lrt
    