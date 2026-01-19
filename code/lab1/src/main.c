#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "base.h"

void read_child_results() {
  FILE *fl;
  char str1[256];

  fl = fopen("res.txt", "r");
  if (!fl) {
    perror("File open error");
    return;
  }

  printf("Results from children:\n");
  while (fgets(str1, sizeof(str1), fl) != NULL) {
    printf("%s", str1);
  }
  fclose(fl);
}

void clean_result() {
  unlink("res.txt");
}

int main() {
  printf("Father started with PID=%d\n", getpid());
  clean_result();

  pid_t child_pid = fork_or_fail();
  if (child_pid == 0) {
    spawn_son(1);
  } else {
    waitpid(child_pid, NULL, 0);
  }

  read_child_results();

  printf("Father finished.\n");
  return 0;
}
