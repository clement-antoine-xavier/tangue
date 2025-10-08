#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int init_server(server_t **server, char binding_address[INET_ADDRSTRLEN], unsigned short binding_port)
{
	*server = (server_t *)malloc(sizeof(server_t));
	if (*server == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Memory allocation failed in init_server\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	(*server)->binding_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ((*server)->binding_file_descriptor < 0)
	{
		fprintf(stderr, "[ERROR] [%s:%d] socket() failed in init_server\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	(*server)->binding_address.sin_family = AF_INET;
	(*server)->binding_address.sin_port = htons(binding_port);

	if (inet_pton(AF_INET, binding_address, &(*server)->binding_address.sin_addr) <= 0)
	{
		fprintf(stderr, "[ERROR] [%s:%d] inet_pton() failed in init_server\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (bind((*server)->binding_file_descriptor, (struct sockaddr *)&(*server)->binding_address, sizeof((*server)->binding_address)) < 0)
	{
		fprintf(stderr, "[ERROR] [%s:%d] bind() failed in init_server\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int terminate_server(server_t *server)
{
	if (close(server->binding_file_descriptor) < 0)
	{
		fprintf(stderr, "[ERROR] [%s:%d] close() failed in terminate_server\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	free(server);

	return EXIT_SUCCESS;
}

int server_receive_data(server_t *server, void *buffer, size_t buffer_size)
{
	ssize_t received_bytes = recv(server->binding_file_descriptor, buffer, buffer_size, 0);
	if (received_bytes < 0)
	{
		fprintf(stderr, "[ERROR] [%s:%d] recv() failed in server_receive_data\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
