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
char* fifo_write_path;
int read_fd = -1;
int write_fd = -1;
volatile sig_atomic_t running = 1;

void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        running = 0;
        printf("\n[ПРОЦЕС 2] Отримано сигнал завершення\n");
    }
}

void* reader_thread(void* arg) {
    char buffer[BUFFER_SIZE];

    printf("[ПРОЦЕС 2] Нитка читання запущена\n");
    printf("[ПРОЦЕС 2] Відкриття FIFO для читання: %s\n", fifo_read_path);

    read_fd = open(fifo_read_path, O_RDONLY);
    if (read_fd < 0) {
        perror("[ПРОЦЕС 2] Помилка відкриття FIFO для читання");
        pthread_exit(NULL);
    }

    printf("[ПРОЦЕС 2] FIFO для читання успішно відкрито\n");
    printf("[ПРОЦЕС 2] Відкриття FIFO для запису: %s\n", fifo_write_path);

    write_fd = open(fifo_write_path, O_WRONLY);
    if (write_fd < 0) {
        perror("[ПРОЦЕС 2] Помилка відкриття FIFO для запису");
        close(read_fd);
        pthread_exit(NULL);
    }

    printf("[ПРОЦЕС 2] FIFO для запису успішно відкрито\n");
    printf("[ПРОЦЕС 2] Готовий до роботи (автоматичне переспрямування)\n");

    while (running) {
        ssize_t bytes_read = read(read_fd, buffer, BUFFER_SIZE);

        if (bytes_read < 0) {
            if (running) {
                perror("[ПРОЦЕС 2] Помилка читання з FIFO");
            }
            break;
        }

        if (bytes_read == 0) {
            printf("[ПРОЦЕС 2] Процес 1 завершив передачу даних (EOF)\n");
            break;
        }

        printf("[ПРОЦЕС 2] 1→2: прийнято \"%s\" (%zd байт)\n", buffer, bytes_read);

        ssize_t bytes_written = write(write_fd, buffer, bytes_read);

        if (bytes_written < 0) {
            perror("[ПРОЦЕС 2] Помилка запису в FIFO");
            break;
        }

        printf("[ПРОЦЕС 2] 2→3: переспрямовано \"%s\" (%zd байт)\n", buffer, bytes_written);
    }

    printf("[ПРОЦЕС 2] Нитка читання завершена\n");

    if (read_fd >= 0) {
        close(read_fd);
    }
    if (write_fd >= 0) {
        close(write_fd);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Використання: %s <fifo_read_path> <fifo_write_path>\n", argv[0]);
        exit(1);
    }

    fifo_read_path = argv[1];
    fifo_write_path = argv[2];

    printf("=== ПРОЦЕС 2 ЗАПУЩЕНО ===\n");
    printf("[ПРОЦЕС 2] FIFO для читання: %s\n", fifo_read_path);
    printf("[ПРОЦЕС 2] FIFO для запису: %s\n", fifo_write_path);
    printf("[ПРОЦЕС 2] Роль: Прийом від процесу 1 → Переспрямування до процесу 3\n");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    pthread_t reader_tid;

    if (pthread_create(&reader_tid, NULL, reader_thread, NULL) != 0) {
        perror("[ПРОЦЕС 2] Помилка створення нитки читання");
        exit(1);
    }

    pthread_join(reader_tid, NULL);

    printf("[ПРОЦЕС 2] Завершення роботи\n");

    return 0;
}
