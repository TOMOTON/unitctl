
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "args.h"
#include "colors.h"
#include "utils.h"


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

int http_get(char const *unix_socket_path, char const *request_target) {
    char const *request_template = "GET %s HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Connection: close\r\n"
                    "\r\n";
    int length = strlen(request_template) -1 + strlen(request_target);
    char *request = malloc(sizeof(char) * length);
    snprintf(request, length, request_template, request_target);
    if (g_options.debug) fprintf(stdout, GREEN "Sending HTTP Request:" NO_COLOR "\n" WHITE "-->>" NO_COLOR "\n%s" WHITE"--<<-" NO_COLOR "\n", request);
    char response[8192] = {0};
    int result = send_request(unix_socket_path, request, NULL, response);
    free(request);
    if (result == 0) {
        if (g_options.debug) fprintf(stdout, GREEN "Received HTTP Response:" NO_COLOR "\n" WHITE "-->>" NO_COLOR "\n%s" WHITE"--<<-" NO_COLOR "\n", response);
    }
    return result;
}


int http_put(char const *unix_socket_path, char const *request_target, struct Entity *entity) {
    char const *request_template = "PUT %s HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Connection: close\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %d\r\n"
                    "\r\n";
    int length = strlen(request_template) - 3 + strlen(request_target) + count_digits(entity->length);
    char *request = malloc(sizeof(char) * length);
    snprintf(request, length, request_template, request_target, entity->length);
    if (g_options.debug) fprintf(stdout, GREEN "Sending HTTP Request:" NO_COLOR "\n" WHITE "-->>" NO_COLOR "\n%s" WHITE"--<<-" NO_COLOR "\n", request);
    char response[8192] = {0};
    int result = send_request(unix_socket_path, request, entity, response);
    free(request);
    if (result == 0) {
        if (g_options.debug) fprintf(stdout, GREEN "Received HTTP Response:" NO_COLOR "\n" WHITE "-->>" NO_COLOR "\n%s" WHITE"--<<-" NO_COLOR "\n", response);
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
    int retry = 10;
    int result = 0;
    int ready;
    char const *request = "GET / HTTP/1.1\r\n\r\n";
    char response[8192] = {0};
    while((ready = send_request(unix_socket_path, request, NULL, response)) != 0) {
    //while((ready = http_get(unix_socket_path, "/")) != 0) {
        sleep(1);
        retry--;
        if (retry == 0) {
            result = -1;
            fprintf(stderr, RED "Error! Process unitd did not respond on '%s'!\n" NO_COLOR, unix_socket_path);
            break;
        }
    }
    return result;
}


int configure_unitd(char const *unix_socket_path, char const *request_target, char const *file_path) {
    struct Entity entity;
    load_entity(file_path, &entity);
    http_put(unix_socket_path, request_target, &entity);
    free(entity.content);
    http_get(unix_socket_path, request_target);
    return 0;
}
