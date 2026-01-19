#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

char* fifo_write_path;
int write_fd = -1;
volatile sig_atomic_t running = 1;

void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        running = 0;
        printf("\n[ПРОЦЕС 1] Отримано сигнал завершення\n");
    }
}

void* writer_thread(void* arg) {
    char buffer[BUFFER_SIZE];

    printf("[ПРОЦЕС 1] Нитка запису запущена\n");
    printf("[ПРОЦЕС 1] Відкриття FIFO для запису: %s\n", fifo_write_path);

    write_fd = open(fifo_write_path, O_WRONLY);
    if (write_fd < 0) {
        perror("[ПРОЦЕС 1] Помилка відкриття FIFO для запису");
        pthread_exit(NULL);
    }

    printf("[ПРОЦЕС 1] FIFO успішно відкрито\n");
    printf("[ПРОЦЕС 1] Введіть рядки для відправки ('exit' або Ctrl+D для завершення):\n");

    while (running) {
        fflush(stdout);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            printf("\n[ПРОЦЕС 1] Завершення введення (EOF)\n");
            break;
        }

        if (strncmp(buffer, "exit", 4) == 0 || strncmp(buffer, "quit", 4) == 0) {
            printf("[ПРОЦЕС 1] Команда завершення отримана\n");
            break;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        if (len == 0) {
            continue;
        }

        ssize_t bytes_written = write(write_fd, buffer, len + 1);

        if (bytes_written < 0) {
            perror("[ПРОЦЕС 1] Помилка запису в FIFO");
            break;
        }

        printf("[ПРОЦЕС 1] 1→2: передано \"%s\" (%zd байт)\n", buffer, bytes_written);
    }

    printf("[ПРОЦЕС 1] Нитка запису завершена\n");

    if (write_fd >= 0) {
        close(write_fd);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Використання: %s <fifo_write_path>\n", argv[0]);
        exit(1);
    }

    fifo_write_path = argv[1];

    printf("=== ПРОЦЕС 1 ЗАПУЩЕНО ===\n");
    printf("[ПРОЦЕС 1] FIFO для запису: %s\n", fifo_write_path);
    printf("[ПРОЦЕС 1] Роль: Читання з клавіатури → Відправка до процесу 2\n");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    pthread_t writer_tid;

    if (pthread_create(&writer_tid, NULL, writer_thread, NULL) != 0) {
        perror("[ПРОЦЕС 1] Помилка створення нитки запису");
        exit(1);
    }

    pthread_join(writer_tid, NULL);

    printf("[ПРОЦЕС 1] Завершення роботи\n");

    return 0;
}
