#include "base.h"


int spawn_son(int level) {
  char level_str[10];
  sprintf(level_str, "%d", level);
  if (execl("./build/son", "son", level_str, NULL) < 0) {
    return -1;
  }
  return 0;
}

pid_t create_child_process(int level) {
  pid_t pw = fork();
  if (pw == 0) {
    return spawn_son(level) == 0 ? pw : -1;
  } else if (pw < 0) {
    return -1;
  }
  return pw;
}


pid_t create_child_process_or_fail(int level) {
    pid_t pw = create_child_process(level);
    if (pw < 0) {
      perror("fork failed");
      exit(1);
    }
    return pw;
}
