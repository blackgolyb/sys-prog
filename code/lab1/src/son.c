#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "base.h"

int parse_arguments(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: son <level>\n");
    exit(1);
  }
  return atoi(argv[1]);
}

void write_info_to_file(int level, pid_t ppid, pid_t mypid, pid_t child_pid) {
  char filename[20];
  sprintf(filename, "res.txt");

  FILE *fl = fopen(filename, "a");
  if (!fl) {
    perror("File open error");
    exit(1);
  }
  fprintf(fl, "Hello, Son! My Father ID=%d, My ID=%d, Your ID=%d, Level=%d\n",
          ppid, mypid, child_pid, level);
  fclose(fl);
}

int main(int argc, char *argv[]) {
  int level = parse_arguments(argc, argv);
  pid_t mypid = getpid();
  pid_t ppid = getppid();

  if (level > N)
    return 0;

  pid_t child_pid = fork_or_fail();
  if (child_pid == 0) {
    spawn_son(level + 1);
  } else {
    write_info_to_file(level, ppid, mypid, child_pid);
    waitpid(child_pid, NULL, 0);
  }

  return 0;
}
