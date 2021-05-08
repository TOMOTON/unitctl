
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


typedef void (*on_await_unitd)(int);


void reconfigure_unitd(int signum, void (*on_await_unitd)(int)) {
    char* unix_socket_path = concat(g_options.run_dir, CONTROL_UNIT_SOCK);
    if(await_unitd(unix_socket_path) == 0) {
        if (on_await_unitd != NULL) {
            on_await_unitd(signum);
        }
        configure_unitd(unix_socket_path, CONFIG_PATH, g_options.file_name);
        print_success("Reconfiguration done with", g_options.file_name);
    } else {
        char *subject = malloc((strlen(g_options.run_dir) + 1) * sizeof(char));
        sprintf(subject, "%s/unit.log", g_options.run_dir);
        print_error("Check Nginx Unit logs in", subject);
        free(subject);
    }
    free(unix_socket_path);
}


void sigint_handler(int signum) {
    (void)signum;
    printf("\n"); // Force break in standard output.
}


void sigusr1_handler(int signum) {
    (void)signum;
    reconfigure_unitd(signum, NULL);
}


void main_await_unitd(int signum) {
    (void)signum;
    fprintf(stdout, YELLOW "--<<-" NO_COLOR "\n");
    signal(SIGUSR1, sigusr1_handler);
    print_info("Installed reconfigure handler for signal", "SIGUSR1");
}


int spawn_unitd() {
    int link[2];
    pid_t pid;

    if (pipe(link) == -1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if (pid == 0) { // Child
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        //execl("/usr/sbin/unitd", "unitd", "--no-daemon", "--control", "unix:/var/run/control.unit.sock", (char *)0);
        //char * argv_list[] = { "unitd", "--no-daemon", "--control", "unix:/var/run/control.unit.sock" };
        char *socket_path = concat(g_options.run_dir, CONTROL_UNIT_SOCK);
        char *unix_socket = concat("unix:", socket_path);
        char *pid_path = concat(g_options.run_dir, UNIT_PID);
        char *log_path = concat(g_options.run_dir, UNIT_LOG);
        char *argv_list[] = { "unitd",
                              "--no-daemon",
                              "--control", unix_socket,
                              "--pid", pid_path,
                              "--log", log_path,
                              "--state", g_options.run_dir,
                              (char *)0
                            };
                            //   "--user", "vagrant",
                            //   "--group", "vagrant"
                            // };
        execv(USR_SBIN_UNITD, argv_list);
        die("execl");
        free(argv_list);
        free(log_path);
        free(pid_path);
        free(unix_socket);
        free(socket_path);
    } else { // Parent
        close(link[1]);
        signal(SIGINT, sigint_handler);
        print_info("Unit run dir is", g_options.run_dir);
        fprintf(stdout, "Output unitd:\n" YELLOW "-->>" NO_COLOR "\n");
        reconfigure_unitd(0, &main_await_unitd);

        int child_status;
        waitpid(pid, &child_status, 0); // Parent process waits here for child to terminate.

        // char foo[4096 + 1];
        // memset(foo, 0, 4096);
        // Single
        // int nbytes = read(link[0], foo, sizeof(foo));
        // printf("Output: (%.*s)\n", nbytes, foo);
        // Continuous
        // int nbytes;
        // while (0 != (nbytes = read(link[0], foo, sizeof(foo)))) {
        //     //totalStr = totalStr + foo;
        //     printf("Child stdout:\n>>>---\n%.*s\n---<<<\n", nbytes, foo);
        //     memset(foo, 0, 4096);
        // }

        if (child_status == 0) { // Verify child process terminated without error
            print_success("The unitd process terminated normally with exit code", "0");
        } else {
            //int code_length = ;
            char *subject = malloc((child_status + 1) * sizeof(char));
            sprintf(subject, "%d", child_status);
            print_error("The unitd process terminated with error code", subject);
            free(subject);
        }
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
//     fprintf(stdout, YELLOW "no color: %d\n" NO_COLOR, g_options.no_color);
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
        spawn_unitd();
    } else {
        print_error("Invald config file", g_options.file_name);
        return ENOENT;
    }

    return EXIT_SUCCESS;
}
