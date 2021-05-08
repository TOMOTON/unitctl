
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "colors.h"


void print_success(char *message, char *subject) {
    if (g_options.no_color) {
        fprintf(stderr, "Success! %s '%s'!\n", message, subject);
    } else {
        fprintf(stderr, GREEN "Success!" NO_COLOR " %s " CYAN "%s" NO_COLOR ".\n", message, subject);
    }
}


void print_info(char *message, char *subject) {
    if (g_options.no_color) {
        fprintf(stderr, "%s '%s'!\n", message, subject);
    } else {
        fprintf(stderr, "%s " CYAN "%s" NO_COLOR ".\n", message, subject);
    }
}


void print_error(char *message, char *subject) {
    if (g_options.no_color) {
        fprintf(stderr, "Error! %s '%s'!\n", message, subject);
    } else {
        fprintf(stderr, RED "Error!" NO_COLOR " %s " CYAN "%s" NO_COLOR "!\n", message, subject);
    }
}


char* concat(char const *s1, char const *s2) {
    int length = strlen(s1) + strlen(s2) + 1;
    char* result = malloc(sizeof(char) * length);
    snprintf(result, length, "%s%s", s1, s2);
    //fprintf(stdout, "utils::concat -> %s!\n", result);
    return result;
}

// alternative (int)((ceil(log10(num))+1)*sizeof(char))
int count_digits(long n) {
    int count = 0;
    while (n != 0) {
        n = n / 10;
        ++count;
    }
    return count;
}