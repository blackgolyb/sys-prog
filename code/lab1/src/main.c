#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "base.h"



void read_child_results() {
  FILE *fl;
  char str1[256];

  for (int i = 1; i <= N; i++) {
    char filename[20];
    sprintf(filename, "son%d.txt", i);

    fl = fopen(filename, "r");
    if (!fl) {
      perror("File open error");
      continue;
    }
    if (fgets(str1, sizeof(str1), fl) != NULL) {
      printf("Father read (%s): %s", filename, str1);
    }
    fclose(fl);
  }
}

void clean_up_child_files() {
  FILE *fl;
  char str1[256];

  for (int i = 1; i <= N; i++) {
    char filename[20];
    sprintf(filename, "son%d.txt", i);

    unlink(filename);
  }
}

int main() {
  printf("Father started with PID=%d\n", getpid());

  pid_t child_pid = create_child_process_or_fail(1);
  waitpid(child_pid, NULL, 0);

  read_child_results();
  // clean_up_child_files();

  printf("Father finished.\n");
  return 0;
}
