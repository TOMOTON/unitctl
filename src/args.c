
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "messages.h"
#include "args.h"
#include "colors.h"
#include "utils.h"


/*
 * Sets the default options
 */
static void set_default_options(options_t* options) {
    options->help = false;
    options->version = false;
    options->use_colors = true;
    options->run_dir[0] = 0;
}


/*
 * Finds the matching case of the current command line option
 */
void switch_options (int arg, options_t* options) {
    switch (arg) {
        case 'h':
            options->help = true;
            help();
            exit(EXIT_SUCCESS);

        case 'v':
            options->version = true;
            version();
            exit(EXIT_SUCCESS);

        case 'r':
            //options->run_dir = optarg;
            fprintf(stdout, "args::switch_options -> optarg=%s!\n", optarg);
            char* run_dir = concat(optarg, "/");
            strncpy(options->run_dir, run_dir, MAX_PATH_SIZE);
            free(run_dir);
            break;

        case 0:
            options->use_colors = false;
            break;

        case '?':
            usage();
            exit(EXIT_FAILURE);

        default:
            usage();
            abort();
    }
}


/*
 * Tries to get the file name. Otherwise, gets stdin
 */
void get_file_name(int argc, char* argv[], options_t* options) {

    /* If there is more arguments, probably, it is an input file */
    if (optind < argc) {
        strncpy(options->file_name, argv[optind++], MAX_PATH_SIZE);

    /* Otherwise, assumes stdin as the input file */
    } else {
        strncpy(options->file_name, "-", MAX_PATH_SIZE);
    }
}


void get_run_dir(int argc, char* argv[], options_t* options) {
    (void)argc;
    (void)argv;
    char *home = getenv("HOME");
    if (home == NULL || strlen(home) == 0) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    printf("Current home dir: %s\n", home);

    char* run_dir = concat(home, "/.unitd/");
    printf("Current run dir: %s\n", run_dir);
    strncpy(options->run_dir, run_dir, MAX_PATH_SIZE);
    free(run_dir);
}


/*
 * Public function that loops until command line options were parsed
 */
void options_parser(int argc, char* argv[], options_t* options) {
    set_default_options(options);

    int arg; /* Current option */

    /* getopt allowed options */
    static struct option long_options[] =
    {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"run-dir", required_argument, 0, 'r'},
        {"no-colors", no_argument, 0, 0},
    };

    while (true) {

        int option_index = 0;
        arg = getopt_long(argc, argv, "hvr:t:", long_options, &option_index);

        /* End of the options? */
        if (arg == -1) break;

        /* Find the matching case of the argument */
        switch_options(arg, options);
    }

    if (options->run_dir[0] == 0) {
        get_run_dir(argc, argv, options);
        struct stat st = {0};
        if (stat(options->run_dir, &st) == -1) {
            mkdir(options->run_dir, 0777);
        }
    }

    /* Gets the file name or exits with error */
    get_file_name(argc, argv, options);
}
