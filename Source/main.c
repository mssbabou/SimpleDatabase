#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 80
#define BUFFER_SIZE 1024

// Function to read a line from the socket
ssize_t read_line(int client_fd, char *buffer, size_t max_length)
{
    size_t i = 0;
    char c;
    while (i < max_length - 1 && read(client_fd, &c, 1) == 1)
    {
        buffer[i++] = c;
        if (c == '\n')
            break;
    }
    buffer[i] = '\0';
    return i;
}

// Function to read a fixed number of bytes from the socket
ssize_t read_fixed(int client_fd, char *buffer, size_t length)
{
    size_t total_read = 0;
    ssize_t bytes_read;

    while (total_read < length)
    {
        bytes_read = read(client_fd, buffer + total_read, length - total_read);
        if (bytes_read <= 0)
            return bytes_read;
        total_read += bytes_read;
    }
    return total_read;
}

// Function to handle an HTTP request
void handle_request(int client_fd)
{
    char buffer[BUFFER_SIZE];
    char *data = NULL;
    size_t content_length = 0;
    int is_chunked = 0;

    // Read headers
    while (read_line(client_fd, buffer, BUFFER_SIZE) > 0)
    {
        if (strcmp(buffer, "\r\n") == 0)
            break;

        if (strncasecmp(buffer, "Content-Length:", 15) == 0)
        {
            content_length = strtol(buffer + 15, NULL, 10);
        }
        else if (strncasecmp(buffer, "Transfer-Encoding: chunked", 26) == 0)
        {
            is_chunked = 1;
        }
    }

    // Read body based on Content-Length or chunked encoding
    if (content_length > 0)
    {
        data = malloc(content_length + 1);
        if (read_fixed(client_fd, data, content_length) > 0)
        {
            data[content_length] = '\0';
            printf("Body: %s\n", data);
        }
    }
    else if (is_chunked)
    {
        size_t total_data_length = 0;
        while (1)
        {
            // Read chunk size
            read_line(client_fd, buffer, BUFFER_SIZE);
            size_t chunk_size = strtol(buffer, NULL, 16);

            if (chunk_size == 0)
                break;

            // Read chunk data
            data = realloc(data, total_data_length + chunk_size + 1);
            read_fixed(client_fd, data + total_data_length, chunk_size);
            total_data_length += chunk_size;

            // Read the trailing CRLF
            read_line(client_fd, buffer, BUFFER_SIZE);
        }
        data[total_data_length] = '\0';
        printf("Body: %s\n", data);
    }

    free(data);
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected...\n");

    handle_request(client_fd);

    close(client_fd);
    close(server_fd);
    return 0;
}
