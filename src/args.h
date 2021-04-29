
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#ifndef ARGS_H
#define ARGS_H


#include <stdbool.h>
#include <getopt.h>


/* Max size of a file name */
#define MAX_PATH_SIZE 512


/* Defines the command line allowed options struct */
struct options {
    bool help;
    bool version;
    bool use_colors;
    bool debug;
    char file_name[MAX_PATH_SIZE];
    char run_dir[MAX_PATH_SIZE];
};


/* Exports options as a global type */
typedef struct options options_t;


extern options_t g_options;


/* Public functions section */
void options_parser(int argc, char* argv[], options_t* options);


#endif // ARGS_H
