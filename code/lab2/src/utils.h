#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>

extern const char *fg_colors[];
extern const char *bg_colors[];
extern const int COLORS;
extern const char *color_reset;

void print_tag(const char tag, int id, bool bg);

void print_producer_tag(int id);

void print_consumer_tag(int id);

void msleep(int ms);

int randint(int start, int end);
int randint(int end);


pthread_t *create_n_threads(
    int n,
    void *(*worker)(void *),
    const std::function<void*(int)>& argsFactory
);

#endif // UTILS_H
