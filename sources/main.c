#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>

#include "packet.h"

void print_usage(const char *program_name)
{
	printf("Usage: %s [OPTIONS]\n", program_name);
	printf("UDP server for receiving and parsing telemetry packets\n\n");
	printf("Options:\n");
	printf("  -p, --port PORT     Port to listen on (required, 1-65535)\n");
	printf("  -v, --verbose       Enable verbose output\n");
	printf("  -h, --help         Show this help message\n\n");
	printf("Examples:\n");
	printf("  %s -p 12345\n", program_name);
	printf("  %s --port 8080 --verbose\n", program_name);
}

void print_packet_info(const packet_t *packet)
{
	printf("=== Packet Info ===\n");
	printf("Race On: %s\n", packet->is_race_on ? "Yes" : "No");
	printf("Timestamp: %u ms\n", packet->timestamp_ms);
	printf("Speed: %.2f m/s (%.2f km/h)\n", packet->speed, packet->speed * 3.6f);
	printf("Engine RPM: %.0f / %.0f\n", packet->engine.current_rpm, packet->engine.max_rpm);
	printf("Position: (%.2f, %.2f, %.2f)\n", packet->position.x, packet->position.y, packet->position.z);
	printf("Gear: %d\n", packet->vehicle_control.gear);
	printf("Fuel: %.1f%%\n", packet->fuel * 100.0f);
	if (packet->is_race_on) {
		printf("Lap: %d\n", packet->race.lap_number);
		printf("Position: %d\n", packet->race.position);
		printf("Current Lap Time: %.3f s\n", packet->race.current_lap);
		printf("Best Lap: %.3f s\n", packet->race.best_lap);
	}
	printf("==================\n\n");
}

int parse_packet(const unsigned char *buffer, size_t buffer_size, packet_t *packet)
{
	if (buffer_size < sizeof(packet_t)) {
		fprintf(stderr, "Buffer too small: %zu bytes, expected at least %zu bytes\n",
				buffer_size, sizeof(packet_t));
		return -1;
	}

	// Direct memory copy (assuming same endianness)
	memcpy(packet, buffer, sizeof(packet_t));
	return 0;
}

int create_udp_server(int port)
{
	int sockfd;
	struct sockaddr_in server_addr;

	// Create UDP socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("Socket creation failed");
		return -1;
	}

	// Allow address reuse
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt failed");
		close(sockfd);
		return -1;
	}

	// Configure server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	// Bind socket to address
	if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		close(sockfd);
		return -1;
	}

	printf("UDP server listening on port %d\n", port);
	return sockfd;
}

void run_server(int sockfd, int verbose)
{
	unsigned char buffer[2048];
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	packet_t packet;
	ssize_t bytes_received;

	printf("Server running... Press Ctrl+C to stop\n\n");

	while (1) {
		// Receive data from client
		bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
								(struct sockaddr *)&client_addr, &client_len);

		if (bytes_received < 0) {
			if (errno == EINTR) {
				continue; // Interrupted by signal, continue
			}
			perror("recvfrom failed");
			break;
		}

		if (verbose) {
			printf("Received %zd bytes from %s:%d\n",
				   bytes_received, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		}

		// Parse the packet
		if (parse_packet(buffer, bytes_received, &packet) == 0) {
			if (verbose) {
				print_packet_info(&packet);
			} else {
				// Brief output in non-verbose mode
				printf("Packet received - Speed: %.1f km/h, RPM: %.0f, Gear: %d\n",
					   packet.speed * 3.6f, packet.engine.current_rpm, packet.vehicle_control.gear);
			}
		} else {
			printf("Failed to parse packet\n\n");
		}
	}
}

int main(const int argc, const char *argv[], const char *envp[])
{
	int port = -1;
	int verbose = 0;
	int sockfd;
	int opt;

	// Define long options
	static struct option long_options[] = {
		{"port", required_argument, 0, 'p'},
		{"verbose", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	// Parse command line arguments
	while ((opt = getopt_long(argc, (char *const *)argv, "p:vh", long_options, NULL)) != -1) {
		switch (opt) {
			case 'p':
				port = atoi(optarg);
				if (port <= 0 || port > 65535) {
					fprintf(stderr, "Error: Port must be between 1 and 65535\n");
					return EXIT_FAILURE;
				}
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				print_usage(argv[0]);
				return EXIT_SUCCESS;
			case '?':
				// getopt_long already printed error message
				print_usage(argv[0]);
				return EXIT_FAILURE;
			default:
				fprintf(stderr, "Unexpected option: %c\n", opt);
				print_usage(argv[0]);
				return EXIT_FAILURE;
		}
	}

	// Check if port was specified
	if (port == -1) {
		fprintf(stderr, "Error: Port is required\n\n");
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (verbose) {
		printf("Starting UDP server with verbose output enabled\n");
	}

	// Create UDP server
	sockfd = create_udp_server(port);
	if (sockfd < 0) {
		return EXIT_FAILURE;
	}

	// Run the server
	run_server(sockfd, verbose);

	// Cleanup
	close(sockfd);
	return EXIT_SUCCESS;
}
