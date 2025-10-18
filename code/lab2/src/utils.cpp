#include "utils.h"

// ANSI escape codes for colors
const char *fg_colors[] = {
    "\033[31m", // Red
    "\033[32m", // Green
    "\033[33m", // Yellow
    "\033[34m", // Blue
    "\033[35m", // Magenta
    "\033[36m", // Cyan
};

const char *bg_colors[] = {
    "\033[41m", // Red BG
    "\033[42m", // Green BG
    "\033[43m", // Yellow BG
    "\033[44m", // Blue BG
    "\033[45m", // Magenta BG
    "\033[46m", // Cyan BG
};
const int COLORS = 6;

const char *color_reset = "\033[0m";
void print_tag(const char tag, int id, bool bg) {
  int attr = id % COLORS;
  const char *start_art = bg ? bg_colors[attr] : fg_colors[attr];
  std::printf("%s[%s%02d]%s", start_art, &tag, id, color_reset);
}

void print_producer_tag(int id) { print_tag('P', id, true); }

void print_consumer_tag(int id) { print_tag('C', id, false); }

void msleep(int ms) { usleep(ms * 1000); }

int randint(int start, int end) {
    int total = end - start;
    if (!total) return rand() % end;
    return start + (rand() % total);
}
int randint(int end) { return rand() % end; }


pthread_t *create_n_threads(
    int n,
    void *(*worker)(void *),
    const std::function<void*(int)>& argsFactory
) {
  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * n);
  for (int i = 0; i < n; ++i) {
    if (pthread_create(&threads[i], nullptr, worker,
                       (void *)argsFactory(i + 1)) != 0) {
      return NULL;
    }
  }
  return threads;
}
