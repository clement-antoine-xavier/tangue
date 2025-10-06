#pragma once

#include "telemetry.h"
#include <sys/socket.h>
#include <netinet/in.h>

// Network configuration
#define NETWORK_BUFFER_SIZE 2048

// Network context structure
typedef struct {
    int socket_fd;
    int port;
    struct sockaddr_in server_addr;
    int initialized;
} network_context_t;

// Network status enumeration
typedef enum {
    NETWORK_SUCCESS = 0,
    NETWORK_ERROR = -1,
    NETWORK_NO_DATA = 1,
    NETWORK_PARSE_ERROR = 2
} network_status_t;

// Function declarations
int network_init(network_context_t *ctx, int port);
void network_cleanup(network_context_t *ctx);
network_status_t network_receive_telemetry(network_context_t *ctx, telemetry_t *telemetry);
int network_parse_packet(const unsigned char *buffer, size_t buffer_size, telemetry_t *telemetry);
