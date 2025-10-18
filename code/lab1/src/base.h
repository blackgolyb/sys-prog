#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 7

int spawn_son(int level);
pid_t create_child_process(int level);
pid_t create_child_process_or_fail(int level);
