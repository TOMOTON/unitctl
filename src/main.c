
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "args.h"
#include "colors.h"
#include "control.h"
#include "constants.h"
#include "utils.h"


#define die(e)                                           \
    do {                                                 \
        fprintf(stderr, MAGENTA "DIE %s\n" NO_COLOR, e); \
        exit(EXIT_FAILURE);                              \
    } while (0);


/* Global options. */
options_t g_options;
//char *config_file_path;


void sigint_handler(int signum) {
    (void)signum;
    //Return type of the handler function should be void
    //printf("\nInside SIGINT handler function\n");
    printf("\n");
}


void sigusr1_handler(int signum) {
    (void)signum;
    //Return type of the handler function should be void
    char* unix_socket_path = concat(g_options.run_dir, CONTROL_UNIT_SOCK);
    configure_unitd(unix_socket_path, LOCALHOST_CONFIG, g_options.file_name);
    free(unix_socket_path);
    fprintf(stdout, GREEN "Succes!" NO_COLOR " Reconfiguration done.\n" NO_COLOR);
}


int spawn_unitd(char* run_dir, char* file_name) {
    int link[2];
    pid_t pid;
    char foo[4096 + 1];
    memset(foo, 0, 4096);

    if (pipe(link) == -1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if (pid == 0) { // Child
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        //! fprintf(stdout, BLUE "CHILD: BEFORE exec!\n" NO_COLOR);
        //execl("/bin/ls", "ls", "-1", (char *)0);
        //execl("/usr/sbin/unitd", "unitd", "--no-daemon", "--control", "unix:/var/run/control.unit.sock", (char *)0);
        //char * argv_list[] = { "unitd", "--no-daemon", "--control", "unix:/var/run/control.unit.sock" };
        //char *unix_socket = as_unix(CONTROL_UNIT_SOCK);
        char *socket_path = concat(run_dir, CONTROL_UNIT_SOCK);
        char *unix_socket = concat("unix:", socket_path);
        char *pid_path = concat(run_dir, UNIT_PID);
        char *log_path = concat(run_dir, UNIT_LOG);
        //char *state_path = concat(run_dir, );
        char *argv_list[] = { "unitd",
                              "--no-daemon",
                              "--control", unix_socket,
                              "--pid", pid_path,
                              "--log", log_path,
                              "--state", run_dir
                            };
                            //   "--user", "vagrant",
                            //   "--group", "vagrant"
                            // };
        execv("/usr/sbin/unitd", argv_list);
        //! fprintf(stdout, BLUE "CHILD: AFTER exec!\n" NO_COLOR);
        die("execl");
        free(socket_path);
        free(unix_socket);
        free(pid_path);
        free(log_path);
        free(argv_list);
    } else { // Parent
        close(link[1]);
        signal(SIGINT, sigint_handler);
        char* unix_socket_path = concat(run_dir, CONTROL_UNIT_SOCK);
        fprintf(stdout, BLUE "Unit run dir" CYAN " %s\n" NO_COLOR, run_dir);
        //fprintf(stdout, BLUE "Unit socket path" CYAN " %s\n" NO_COLOR, unix_socket_path);
        //fprintf(stdout, BLUE "Config file path %s\n" NO_COLOR, file_name);
        //fprintf(stdout, BLUE "Application URL  %s\n" NO_COLOR, LOCALHOST_CONFIG);
        fprintf(stdout, NO_COLOR "Output unitd:\n-->>\n" NO_COLOR);
        if(await_unitd(unix_socket_path) == 0) {
            fprintf(stdout, NO_COLOR "--<<-\n" NO_COLOR);
            signal(SIGUSR1, sigusr1_handler);
            configure_unitd(unix_socket_path, CONFIG_PATH, file_name);
            fprintf(stdout, GREEN "Succes!" NO_COLOR " Reconfiguration done.\n" NO_COLOR);
            //wait(NULL);
            int returnStatus;
            waitpid(pid, &returnStatus, 0); // Parent process waits here for child to terminate.

            // Single
            // int nbytes = read(link[0], foo, sizeof(foo));
            // printf("Output: (%.*s)\n", nbytes, foo);
            // Continuous
            // int nbytes;
            // while (0 != (nbytes = read(link[0], foo, sizeof(foo)))) {
            //   //totalStr = totalStr + foo;
            //   printf("Child stdout:\n>>>---\n%.*s\n---<<<\n", nbytes, foo);
            //   memset(foo, 0, 4096);
            // }
            if (returnStatus == 0) { // Verify child process terminated without error
                printf("The unitd process terminated normally.\n");
            } else {
                printf("The unitd process terminated with an error!.\n");
            }
        } else {
            fprintf(stdout, RED "Failure!" NO_COLOR " Check Nginx Unit logs in '%s/unit.log'!\n" NO_COLOR, run_dir);
        }
        free(unix_socket_path);
    }
    return 0;
}

int main(int argc, char *argv[]) {

    /* Read command line options */

    options_parser(argc, argv, &g_options);

// #ifdef DEBUG
//     fprintf(stdout, BLUE "Command line g_options:\n" NO_COLOR);
//     fprintf(stdout, YELLOW "help: %d\n" NO_COLOR, g_options.help);
//     fprintf(stdout, YELLOW "version: %d\n" NO_COLOR, g_options.version);
//     fprintf(stdout, YELLOW "use colors: %d\n" NO_COLOR, g_options.use_colors);
//     fprintf(stdout, YELLOW "filename: %s\n" NO_COLOR, g_options.file_name);
// #endif

    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //     printf("Current working dir: %s\n", cwd);
    // } else {
    //     perror("getcwd() error");
    //     return 1;
    // }

    if (access(g_options.file_name, R_OK) == 0) {
        spawn_unitd(g_options.run_dir, g_options.file_name);
    } else {
        fprintf(stdout, RED "Error!" NO_COLOR " Invalid config file " CYAN "'%s'!\n" NO_COLOR, g_options.file_name);
        return ENOENT;
    }

    return EXIT_SUCCESS;
}
