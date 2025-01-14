#ifndef HTTPSERVER_H
#define HTTPSERVER_H

typedef enum 
{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_UNKNOWN
} HttpMethod;

typedef enum 
{
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_UNKNOWN_VERSION
} HttpVersion;

typedef struct 
{
    char *key;
    char *value;
} HttpHeader;

typedef struct 
{
    HttpMethod method;
    char *path;
    HttpVersion version;
    HttpHeader *headers;
    int header_count;
    char *body;
} HttpRequest;

#endif HTTPSERVER_H