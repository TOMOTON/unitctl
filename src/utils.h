
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#ifndef UTILS_H
#define UTILS_H


void print_success(char *message, char *subject);

void print_info(char *message, char *subject);

void print_error(char *message, char *subject);


char* concat(char const *s1, char const *s2);

int count_digits(long n);


#endif // UTILS_H