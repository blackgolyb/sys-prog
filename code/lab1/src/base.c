#include "base.h"


void spawn_son(int level) {
  char level_str[10];
  sprintf(level_str, "%d", level);
  if (execl("./build/son", "son", level_str, NULL) < 0) {
      perror("fork failed");
      exit(1);
  }
}

pid_t fork_or_fail() {
    pid_t pw = fork();
    if (pw < 0) {
      perror("fork failed");
      exit(1);
    }
    return pw;
}
