
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "colors.h"


struct Entity {
    long length;
    char* content;
};


int send_request(char const *unix_socket_path, char const *request, struct Entity *entity, char *response) {
    int fd;
    struct sockaddr_un addr;
    int result = 1;
    int len;

    if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) >= 0) { // SOCK_DGRAM
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, unix_socket_path);
        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != -1) {
            if (send(fd, request, strlen(request), 0) != -1) {
                if (entity != NULL) {
                    //fprintf(stdout, "configure_unitd::send_requests - Sending entity (%ld)\n", entity->length);
                    send(fd, entity->content, entity->length, 0);
                    send(fd, "\r\n", 2, 0);
                }
                if ((len = recv(fd, response, 8192, 0)) >= 0) {
                    //fprintf(stdout, "configure_unitd::send_requests - Received octets=%d\n", len);
                    result = 0;
                } else {
                    perror("recv");
                }
            } else {
                perror("send");
            }
        } else {
           //perror("connect");
        }
        close(fd);
    } else {
        perror("socket");
    }
    return result;
}

int get_url(char const *unix_socket_path, char const *url) {
    (void)url;
    char const *request = "GET /config HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Connection: close\r\n"
                    "\r\n";
    //fprintf(stdout, "configure_unitd::get_url - Sending HTTP Request:\n-->>\n%s--<<-\n", request);
    char response[8192] = {0};
    int result = send_request(unix_socket_path, request, NULL, response);
    if (result == 0) {
        //fprintf(stdout, "configure_unitd::get_url - Received HTTP Response:\n-->>\n%s--<<-\n", response);
    }
    return result;
}


int put_url(char const *unix_socket_path, char const *url, struct Entity *entity) {
    (void)url;
    char const *request = "PUT /config HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Connection: close\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: 283\r\n"
                    "\r\n";
    //fprintf(stdout, "configure_unitd::put_url - Sending HTTP Request:\n-->>\n%s--<<-\n", request);
    char response[8192] = {0};
    int result = send_request(unix_socket_path, request, entity, response);
    if (result == 0) {
        //fprintf(stdout, "configure_unitd::put_url - Received HTTP Response:\n-->>\n%s--<<-\n", response);
    }
    return result;
}


int load_entity(char const *file_path, struct Entity *entity) {
    char *source = NULL;
    //fprintf(stdout, "configure_unitd::load_entity - Reading file: %s\n", file_path);
    int result = -1;
    FILE *fp = fopen(file_path, "r");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long length = ftell(fp);
            if (length != -1) {
                /* Allocate our buffer to that size. */
                source = malloc(sizeof(char) * (length + 1));

                /* Go back to the start of the file. */
                if (fseek(fp, 0L, SEEK_SET) == 0) {
                    /* Read the entire file into memory. */
                    size_t newLen = fread(source, sizeof(char), length, fp);
                    if (ferror(fp) == 0) {
                        source[newLen++] = '\0'; /* Just to be safe. */
                        entity->length = length;
                        entity->content = source;
                        result = 0;
                    } else {
                        fprintf(stderr, RED "Error! Failure reading file '%s'!\n" NO_COLOR, file_path);
                    }
                }
            }
        }
        fclose(fp);
    }
    return result;
}


int await_unitd(char const *unix_socket_path) {
    int retry = 100;
    int result = 0;
    int ready;
    while((ready = get_url(unix_socket_path, "/")) != 0) {
        //fprintf(stdout, MAGENTA "NOT READY.\n" NO_COLOR);
        sleep(1);
        retry--;
        if (retry == 0) {
            result = -1;
            break;
        }
    }
    return result;
}


int configure_unitd(char const *unix_socket_path, char const *url, char const *file_path) {
    struct Entity entity;
    load_entity(file_path, &entity);
    //fprintf(stdout, "configure_unitd::configure_unitd Entity: octets=%ld\n-->>\n%s\n--<<-\n", entity.length, entity.content);
    put_url(unix_socket_path, url, &entity);
    free(entity.content);
    get_url(unix_socket_path, url);
    return 0;
}

// char* build_request(char *hostname, char *request_path) {}
//     char *request = NULL;
//     Buffer *request_buffer = buffer_alloc(BUF_SIZE);

//     buffer_appendf(request_buffer, "GET %s HTTP/1.0\r\n", request_path);
//     buffer_appendf(request_buffer, "Host: %s\r\n", hostname);
//     buffer_appendf(request_buffer, "Connection: close\r\n\r\n");

//     request = buffer_to_s(request_buffer);
//     buffer_free(request_buffer);

//     return request;
// }