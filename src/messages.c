
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <stdio.h>
#include <stdlib.h>

#include "colors.h"
#include "messages.h"


/*
 * Help message
 */
void help () {
    fprintf(stdout, BLUE __PROGRAM_NAME__ "\n\n" NO_COLOR);
    usage();
    description();
    options();
    version();
}


/*
 * Usage message
 */
void usage () {
    fprintf(stdout, YELLOW "Usage: " NO_COLOR);
    fprintf(stdout, "%s [options] PATH\n\n", __PROGRAM_NAME__);
}


/*
 * Description message
 */
void description () {
    fprintf(stdout, YELLOW "Description: " NO_COLOR);
    fprintf(stdout,
        "A supervisor for Nginx Unit. "
        "Use it to control and configure Unit in process isolation environments."
        "\n\n"
    );
}


/*
 * Options message
 */
void options () {
    fprintf(stdout, YELLOW "Options:\n\n" NO_COLOR);
    fprintf(stdout, GRAY "\t-v|--version\n" NO_COLOR
                    "\t\tPrints %s version\n\n", __PROGRAM_NAME__);
    fprintf(stdout, GRAY "\t-h|--help\n" NO_COLOR
                    "\t\tPrints this help message\n\n");
    fprintf(stdout, GRAY "\t-r|--run-dir\n" NO_COLOR
                    "\t\tRun dir for Nginx Unit [default: $HOME/.unitd]\n\n");
    fprintf(stdout, GRAY "\t--no-color\n" NO_COLOR
                    "\t\tDoes not use colors for printing\n\n");
}


/*
 * Version message
 */
void
version() {
    fprintf(stdout, YELLOW "Version: " NO_COLOR);
    fprintf(stdout, GRAY "%s\n" NO_COLOR,
            __PROGRAM_VERSION__);
}
