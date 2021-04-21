#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "args.h"
#include "colors.h"
#include "curl.h"


#define die(e)                      \
  do                                \
  {                                 \
    fprintf(stderr, "DIE %s\n", e); \
    exit(EXIT_FAILURE);             \
  } while (0);


//static const char CONTROL_UNIT_SOCK[] = "/var/run/control.unit.sock";
static char CONTROL_UNIT_SOCK[] = "/home/vagrant/.unitd/control.unit.sock";

void sigint_handler(int signum){
  (void)signum;
  //Return type of the handler function should be void
  printf("\nInside handler function\n");
}

char* as_unix(char *socket) {
    int length = strlen(socket) + 6;
    char *result = malloc(sizeof(char) * length);
    snprintf(result, length, "unix:%s", socket);
    fprintf(stdout, BLUE "as_unix -> %s!\n" NO_COLOR, result);
    return result;
}

int spawn_unit(char *file_name)
{
  int link[2];
  pid_t pid;
  char foo[4096 + 1];
  memset(foo, 0, 4096);

  if (pipe(link) == -1)
    die("pipe");

  if ((pid = fork()) == -1)
    die("fork");

  if (pid == 0) // Child
  {
    dup2(link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    fprintf(stdout, BLUE "CHILD: BEFORE exec!\n" NO_COLOR);
    //execl("/bin/ls", "ls", "-1", (char *)0);
    //execl("/usr/sbin/unitd", "unitd", "--no-daemon", "--control", "unix:/var/run/control.unit.sock", (char *)0);
    //char * argv_list[] = { "unitd", "--no-daemon", "--control", "unix:/var/run/control.unit.sock" };
    char *unix_socket = as_unix(CONTROL_UNIT_SOCK);
    char * argv_list[] = { "unitd",
      "--no-daemon",
      "--control", unix_socket,
      "--pid", "/home/vagrant/.unitd/unit.pid",
      "--log", "/home/vagrant/.unitd/unit.log",
      "--state", "/home/vagrant/.unitd/state",
      "--user", "vagrant",
      "--group", "vagrant"
    };
    execv("/usr/sbin/unitd", argv_list);
    fprintf(stdout, BLUE "CHILD: AFTER exec!\n" NO_COLOR);
    die("execl");
    free(unix_socket);
  }
  else // Parent
  {
    close(link[1]);
    signal(SIGINT, sigint_handler);
    // Single
    // int nbytes = read(link[0], foo, sizeof(foo));
    // printf("Output: (%.*s)\n", nbytes, foo);
    // Continuous
    //std::string totalStr;
    fprintf(stdout, BLUE "HTTP Get: pid=%d\n" NO_COLOR, pid);
    sleep(1);
    http_get_unit(CONTROL_UNIT_SOCK);
    //http_put_unit(".data/config.json");
    fprintf(stdout, BLUE "Config file name %s!\n" NO_COLOR, file_name);
    http_put_unit(CONTROL_UNIT_SOCK, file_name);
    http_get_unit(CONTROL_UNIT_SOCK);

    // int nbytes;
    // while (0 != (nbytes = read(link[0], foo, sizeof(foo))))
    // {
    //   //totalStr = totalStr + foo;
    //   printf("Child stdout:\n>>>---\n%.*s\n---<<<\n", nbytes, foo);
    //   memset(foo, 0, 4096);
    // }

    fprintf(stdout, BLUE "Waiting for child process: pid=%d\n" NO_COLOR, pid);
    //wait(NULL);
    int returnStatus;
    waitpid(pid , &returnStatus, 0); // Parent process waits here for child to terminate.
    if (returnStatus == 0)  // Verify child process terminated without error.
    {
       printf("The Nginx Unit process terminated normally.\n");
    }
    if (returnStatus == 1)
    {
       printf("The Nginx Unit process terminated with an error!.\n");
    }
    fprintf(stdout, BLUE "PARENT: it's OVER!!!\n" NO_COLOR);
  }
  //sleep(3000);
  return 0;
}


int main(int argc, char *argv[])
{

  /* Read command line options */
  options_t options;
  options_parser(argc, argv, &options);

#ifdef DEBUG
  fprintf(stdout, BLUE "Command line options:\n" NO_COLOR);
  fprintf(stdout, YELLOW "help: %d\n" NO_COLOR, options.help);
  fprintf(stdout, YELLOW "version: %d\n" NO_COLOR, options.version);
  fprintf(stdout, YELLOW "use colors: %d\n" NO_COLOR, options.use_colors);
  fprintf(stdout, YELLOW "filename: %s\n" NO_COLOR, options.file_name);
#endif

  if(access(options.file_name, R_OK) == 0) {
    spawn_unit(options.file_name);
  } else {
    fprintf(stdout, RED "Config file %s does not exist!\n" NO_COLOR, options.file_name);
     return ENOENT;
  }

  return EXIT_SUCCESS;
}
