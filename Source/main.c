#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 16384

// Helper function to find the end of headers ("\r\n\r\n")
int find_End_Of_Headers(const char *buffer) {
    const char *end_of_headers = strstr(buffer, "\r\n\r\n");
    return end_of_headers ? (end_of_headers - buffer + 4) : -1;
}

// Function to handle the client request
void handle_Requests(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    char *request = NULL;
    size_t request_size = 0;

    printf("Receiving request...\n");

    // Keep reading data until headers are fully received
    while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';

        // Allocate memory to store the request
        request = realloc(request, request_size + bytes_received + 1);
        memcpy(request + request_size, buffer, bytes_received);
        request_size += bytes_received;
        request[request_size] = '\0';

        // Check if headers are complete
        int header_end = find_End_Of_Headers(request);
        if (header_end != -1) {
            printf("Headers received:\n%s\n", request);

            // Check for Content-Length header
            char *content_length_str = strstr(request, "Content-Length:");
            size_t content_length = 0;
            if (content_length_str) {
                sscanf(content_length_str, "Content-Length: %zu", &content_length);
            }

            // Read the body if there's a Content-Length
            if (content_length > 0) {
                size_t body_received = request_size - header_end;
                while (body_received < content_length) {
                    bytes_received = read(client_fd, buffer, sizeof(buffer));
                    if (bytes_received <= 0) break;

                    request = realloc(request, request_size + bytes_received + 1);
                    memcpy(request + request_size, buffer, bytes_received);
                    request_size += bytes_received;
                    body_received += bytes_received;
                }
                request[request_size] = '\0';
            }

            // Print the full request (headers + body)
            printf("Full request:\n%s\n", request);
            break;
        }
    }

    // Clean up
    free(request);
    printf("Request fully received.\n");
}

int main() {
    // Create the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept a single client connection
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected...\n");
    
    handle_Requests(client_fd);

    // Clean up
    close(client_fd);
    close(server_fd);

    return 0;
}
