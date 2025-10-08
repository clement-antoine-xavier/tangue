#pragma once

#include <arpa/inet.h>

typedef struct server_s
{
	int binding_file_descriptor;
	struct sockaddr_in binding_address;
} server_t;

int init_server(server_t **server, char binding_address[INET_ADDRSTRLEN], unsigned short binding_port);
int terminate_server(server_t *server);
int server_receive_data(server_t *server, void *buffer, size_t buffer_size);
