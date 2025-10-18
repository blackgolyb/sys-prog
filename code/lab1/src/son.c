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

void write_info_to_file(int level, pid_t ppid, pid_t mypid) {
  char filename[20];
  sprintf(filename, "son%d.txt", level);

  FILE *fl = fopen(filename, "w");
  if (!fl) {
    perror("File open error");
    exit(1);
  }
  fprintf(fl, "Hello, Son! My Father ID=%d, My ID=%d, Level=%d\n", ppid, mypid,
          level);
  fclose(fl);
}

void spawn_next_child(int level) {
  if (level < N) {
    pid_t child_pid = create_child_process_or_fail(level + 1);
    waitpid(child_pid, NULL, 0);
  }
}

int main(int argc, char *argv[]) {
  int level = parse_arguments(argc, argv);
  pid_t mypid = getpid();
  pid_t ppid = getppid();

  printf("Son level %d started. PID=%d, Parent=%d\n", level, mypid, ppid);
  write_info_to_file(level, ppid, mypid);
  spawn_next_child(level);

  printf("Son level %d finished.\n", level);
  return 0;
}
