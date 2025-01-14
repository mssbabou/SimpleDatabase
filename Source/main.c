#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "HTTPServer.h"

#define PORT 8080

enum HTTPMethod 
{
    GET,
    POST,
    PUT,
    DELETE
};

struct HTTPRequestHeader
{
    enum HTTPMethod method;
    char path[1024];
    long contentLength;
};

// Returns index in array where pattern begins, or -1 if not found
int FindFirstPatternString(const unsigned char *array, size_t array_len, const unsigned char *pattern, size_t pattern_len)
{
    if (pattern_len == 0 || array_len < pattern_len) {
        return -1;
    }

    for (size_t i = 0; i <= array_len - pattern_len; i++) {
        size_t j;
        for (j = 0; j < pattern_len; j++) {
            if (array[i + j] != pattern[j]) {
                break;
            }
        }

        if (j == pattern_len) {
            return i;
        }
    }

    return -1;
}

void printHex(const unsigned char *data, size_t length)
{
    if (!data) {
        printf("printHex: data is NULL\n");
        return;
    }

    for (size_t i = 0; i < length; i++) {
        printf("%02x ", data[i]);

        if (i < length - 1) {
            printf(" ");
        }
    }

    printf("\n");
}

void PrintCharArray(const unsigned char *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%c", data[i]);
    }
    
}

int main(int argc, char *argv[])
{    
    // Create the socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the Port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept a single client connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected...\n");

    char* data;
    int data_len = 0;
    char buffer[1024];

    HttpRequest httpRequest;
    bool foundHeader = false;
    bool foundBody = false;
    
    ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer));

    if (bytes_received > 0)
    {
        data = (char*)malloc(bytes_received);
        memcpy(data, buffer, bytes_received);
        data_len += bytes_received;


        char headerStopPattern[] = "\r\n\r\n";
        int headerStopIndex = FindFirstPatternString(data, data_len, headerStopPattern, strlen(headerStopPattern));

        if (headerStopIndex == -1)
        {            
            while(1)
            {
                bytes_received = read(client_fd, buffer, sizeof(buffer));

                if (bytes_received > 0)
                {
                    data = (char*)realloc(data, data_len + bytes_received);
                    memcpy(data + data_len, buffer, bytes_received);
                    data_len += bytes_received;

                    PrintCharArray(data, data_len);
                    // Check if data contains all stop flags and break if so

                    // Find Header stop "\r\n\r\n"
                    char headerStopPattern[] = "\r\n\r\n";
                    int headerStopIndex = FindFirstPatternString(data, data_len, headerStopPattern, sizeof(headerStopPattern));

                    if (headerStopIndex != -1)
                    {
                        // Check if Header has already been extracted
                        if (foundHeader)
                        {
                            
                        }
                        else 
                        {
                            // Extract Header
                        }
                    }
                }
                else
                {
                    break;
                }
                
            }
        }
    }

    // Clean up
    close(client_fd);
    close(server_fd);
}