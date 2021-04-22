
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <stdio.h>
#include <string.h>


char* concat(char const *s1, char const *s2) {
    int length = strlen(s1) + strlen(s2) + 1;
    char* result = malloc(sizeof(char) * length);
    snprintf(result, length, "%s%s", s1, s2);
    fprintf(stdout, "utils::concat -> %s!\n", result);
    return result;
}