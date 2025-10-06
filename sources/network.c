#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

int network_init(network_context_t *ctx, int port) {
    if (!ctx) return -1;

    ctx->socket_fd = -1;
    ctx->port = port;
    ctx->initialized = 0;
    memset(&ctx->server_addr, 0, sizeof(ctx->server_addr));

    // Create UDP socket
    ctx->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctx->socket_fd < 0) {
        fprintf(stderr, "Error creating socket: %s\n", strerror(errno));
        return -1;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(ctx->socket_fd, F_GETFL, 0);
    if (flags < 0) {
        fprintf(stderr, "Error getting socket flags: %s\n", strerror(errno));
        close(ctx->socket_fd);
        ctx->socket_fd = -1;
        return -1;
    }

    if (fcntl(ctx->socket_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        fprintf(stderr, "Error setting socket to non-blocking: %s\n", strerror(errno));
        close(ctx->socket_fd);
        ctx->socket_fd = -1;
        return -1;
    }

    // Set up server address
    ctx->server_addr.sin_family = AF_INET;
    ctx->server_addr.sin_addr.s_addr = INADDR_ANY;
    ctx->server_addr.sin_port = htons(port);

    // Bind socket to address
    if (bind(ctx->socket_fd, (struct sockaddr*)&ctx->server_addr, sizeof(ctx->server_addr)) < 0) {
        fprintf(stderr, "Error binding socket to port %d: %s\n", port, strerror(errno));
        close(ctx->socket_fd);
        ctx->socket_fd = -1;
        return -1;
    }

    ctx->initialized = 1;
    return 0;
}

void network_cleanup(network_context_t *ctx) {
    if (!ctx) return;

    if (ctx->socket_fd >= 0) {
        close(ctx->socket_fd);
        ctx->socket_fd = -1;
    }
    ctx->initialized = 0;
}

int network_receive_telemetry(network_context_t *ctx, telemetry_t *telemetry) {
    if (!ctx || !telemetry || ctx->socket_fd < 0) return -1;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    ssize_t bytes_received = recvfrom(ctx->socket_fd, telemetry, sizeof(telemetry_t), 0,
                                      (struct sockaddr*)&client_addr, &client_addr_len);
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available (non-blocking socket)
            return NETWORK_NO_DATA;
        } else {
            fprintf(stderr, "Error receiving data: %s\n", strerror(errno));
            return NETWORK_ERROR;
        }
    }
    if (bytes_received == 0) {
        // Connection closed
        return NETWORK_ERROR;
    }
    if (bytes_received != sizeof(telemetry_t)) {
        fprintf(stderr, "Warning: Received %zd bytes, expected %zu bytes\n",
                bytes_received, sizeof(telemetry_t));
        // Don't return error, just warn and continue
    }
    return NETWORK_SUCCESS;
}
