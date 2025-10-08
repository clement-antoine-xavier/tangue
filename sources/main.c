#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dashboard.h"
#include "gauge.h"

int parse_args(int argc, char *argv[], char *binding_address, size_t address_size, unsigned short *binding_port)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s [binding_address] [binding_port]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (strlen(argv[1]) < address_size)
	{
		strncpy(binding_address, argv[1], address_size);
	}
	else
	{
		fprintf(stderr, "[ERROR] Binding address is too long\n");
		return EXIT_FAILURE;
	}

	*binding_port = (unsigned short)atoi(argv[2]);

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	char binding_address[INET_ADDRSTRLEN] = "0.0.0.0";
	unsigned short binding_port = 0;
	dashboard_t *dashboard = NULL;
	int return_code = EXIT_SUCCESS;

	if (parse_args(argc, argv, binding_address, sizeof(binding_address), &binding_port) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to parse arguments\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (init_dashboard(&dashboard, binding_address, binding_port) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to initialize dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	
	while (dashboard_is_running(dashboard))
	{
		if (dashboard_routine(dashboard) != EXIT_SUCCESS)
		{
			return_code = EXIT_FAILURE;
			fprintf(stderr, "[ERROR] [%s:%d] dashboard_routine failed\n", __FILE__, __LINE__);
			break;
		}
	}

	if (terminate_dashboard(dashboard) != EXIT_SUCCESS)
	{
		return_code = EXIT_FAILURE;
		fprintf(stderr, "[ERROR] [%s:%d] Failed to terminate dashboard\n", __FILE__, __LINE__);
	}

	return return_code;
}
