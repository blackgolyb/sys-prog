#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define FIFO_1_2 "/tmp/fifo_1_2"
#define FIFO_2_3 "/tmp/fifo_2_3"
#define FIFO_3_4 "/tmp/fifo_3_4"

pid_t pids[4];
int num_processes = 0;

void cleanup_fifos() {
    unlink(FIFO_1_2);
    unlink(FIFO_2_3);
    unlink(FIFO_3_4);
}

void cleanup_and_exit(int sig) {
    printf("\n[MANAGER] Отримано сигнал завершення. Зупинка всіх процесів...\n");

    for (int i = 0; i < num_processes; i++) {
        if (pids[i] > 0) {
            kill(pids[i], SIGTERM);
        }
    }

    for (int i = 0; i < num_processes; i++) {
        if (pids[i] > 0) {
            waitpid(pids[i], NULL, 0);
        }
    }

    cleanup_fifos();

    printf("[MANAGER] Всі процеси завершено.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    printf("=== MANAGER: Запуск системи обміну даними через іменовані канали ===\n");
    printf("Схема передачі: 1 → 2 → 3 → 4 (автоматичне переспрямування)\n");
    printf("Для завершення натисніть Ctrl+C\n\n");

    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    cleanup_fifos();

    printf("[MANAGER] Створення іменованих каналів...\n");

    if (mkfifo(FIFO_1_2, S_IRUSR | S_IWUSR) < 0) {
        perror("[MANAGER] Помилка створення FIFO_1_2");
        exit(1);
    }
    printf("[MANAGER] Створено FIFO: %s\n", FIFO_1_2);

    if (mkfifo(FIFO_2_3, S_IRUSR | S_IWUSR) < 0) {
        perror("[MANAGER] Помилка створення FIFO_2_3");
        cleanup_fifos();
        exit(1);
    }
    printf("[MANAGER] Створено FIFO: %s\n", FIFO_2_3);

    if (mkfifo(FIFO_3_4, S_IRUSR | S_IWUSR) < 0) {
        perror("[MANAGER] Помилка створення FIFO_3_4");
        cleanup_fifos();
        exit(1);
    }
    printf("[MANAGER] Створено FIFO: %s\n", FIFO_3_4);

    printf("[MANAGER] Всі іменовані канали створено успішно\n\n");

    printf("[MANAGER] Запуск процесу 1...\n");
    pids[0] = fork();
    if (pids[0] < 0) {
        perror("[MANAGER] Помилка fork для процесу 1");
        cleanup_fifos();
        exit(1);
    }

    if (pids[0] == 0) {
        execl("./build/process1", "process1", FIFO_1_2, NULL);
        perror("[MANAGER] Помилка execl для процесу 1");
        exit(1);
    }
    num_processes++;

    printf("[MANAGER] Запуск процесу 2...\n");
    pids[1] = fork();
    if (pids[1] < 0) {
        perror("[MANAGER] Помилка fork для процесу 2");
        cleanup_fifos();
        exit(1);
    }

    if (pids[1] == 0) {
        execl("./build/process2", "process2", FIFO_1_2, FIFO_2_3, NULL);
        perror("[MANAGER] Помилка execl для процесу 2");
        exit(1);
    }
    num_processes++;

    printf("[MANAGER] Запуск процесу 3...\n");
    pids[2] = fork();
    if (pids[2] < 0) {
        perror("[MANAGER] Помилка fork для процесу 3");
        cleanup_fifos();
        exit(1);
    }

    if (pids[2] == 0) {
        execl("./build/process3", "process3", FIFO_2_3, FIFO_3_4, NULL);
        perror("[MANAGER] Помилка execl для процесу 3");
        exit(1);
    }
    num_processes++;

    printf("[MANAGER] Запуск процесу 4...\n");
    pids[3] = fork();
    if (pids[3] < 0) {
        perror("[MANAGER] Помилка fork для процесу 4");
        cleanup_fifos();
        exit(1);
    }

    if (pids[3] == 0) {
        execl("./build/process4", "process4", FIFO_3_4, NULL);
        perror("[MANAGER] Помилка execl для процесу 4");
        exit(1);
    }
    num_processes++;

    printf("[MANAGER] Всі процеси запущено. Система працює...\n\n");

    for (int i = 0; i < num_processes; i++) {
        int status;
        pid_t finished_pid = wait(&status);
        printf("[MANAGER] Процес з PID %d завершено\n", finished_pid);
    }

    cleanup_fifos();

    printf("\n[MANAGER] Всі процеси завершили роботу. Вихід.\n");

    return 0;
}
