#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

char* fifo_read_path;
int read_fd = -1;
volatile sig_atomic_t running = 1;

void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        running = 0;
        printf("\n[ПРОЦЕС 4] Отримано сигнал завершення\n");
    }
}

void* reader_thread(void* arg) {
    char buffer[BUFFER_SIZE];

    printf("[ПРОЦЕС 4] Нитка читання запущена\n");
    printf("[ПРОЦЕС 4] Відкриття FIFO для читання: %s\n", fifo_read_path);

    read_fd = open(fifo_read_path, O_RDONLY);
    if (read_fd < 0) {
        perror("[ПРОЦЕС 4] Помилка відкриття FIFO для читання");
        pthread_exit(NULL);
    }

    printf("[ПРОЦЕС 4] FIFO для читання успішно відкрито\n");
    printf("[ПРОЦЕС 4] Готовий до прийому даних\n");

    while (running) {
        ssize_t bytes_read = read(read_fd, buffer, BUFFER_SIZE);

        if (bytes_read < 0) {
            if (running) {
                perror("[ПРОЦЕС 4] Помилка читання з FIFO");
            }
            break;
        }

        if (bytes_read == 0) {
            printf("[ПРОЦЕС 4] Процес 3 завершив передачу даних (EOF)\n");
            break;
        }

        printf("[ПРОЦЕС 4] 3→4: прийнято \"%s\" (%zd байт)\n", buffer, bytes_read);
        printf("[ПРОЦЕС 4] КІНЦЕВИЙ РЕЗУЛЬТАТ: \"%s\"\n", buffer);
        printf("─────────────────────────────────────────────\n");
    }

    printf("[ПРОЦЕС 4] Нитка читання завершена\n");

    if (read_fd >= 0) {
        close(read_fd);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Використання: %s <fifo_read_path>\n", argv[0]);
        exit(1);
    }

    // Отримуємо шлях до FIFO для читання
    fifo_read_path = argv[1];

    printf("=== ПРОЦЕС 4 ЗАПУЩЕНО ===\n");
    printf("[ПРОЦЕС 4] FIFO для читання: %s\n", fifo_read_path);
    printf("[ПРОЦЕС 4] Роль: Прийом від процесу 3 → Виведення результату\n");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    pthread_t reader_tid;

    if (pthread_create(&reader_tid, NULL, reader_thread, NULL) != 0) {
        perror("[ПРОЦЕС 4] Помилка створення нитки читання");
        exit(1);
    }

    pthread_join(reader_tid, NULL);

    printf("[ПРОЦЕС 4] Завершення роботи\n");

    return 0;
}
