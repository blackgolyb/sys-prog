#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 256

// Глобальні змінні
int read_fd;
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
    
    while (running) {
        ssize_t bytes_read = read(read_fd, buffer, BUFFER_SIZE);
        
        if (bytes_read < 0) {
            if (running) {
                perror("[ПРОЦЕС 4] Помилка читання з каналу");
            }
            break;
        }
        
        if (bytes_read == 0) {
            // EOF - процес 3 закрив канал
            printf("[ПРОЦЕС 4] Процес 3 завершив передачу даних\n");
            break;
        }
        
        printf("[ПРОЦЕС 4] 3→4: прийнято \"%s\" (%zd байт)\n", buffer, bytes_read);
        printf("[ПРОЦЕС 4] КІНЦЕВИЙ РЕЗУЛЬТАТ: \"%s\"\n", buffer);
        printf("─────────────────────────────────────────────\n");
    }
    
    printf("[ПРОЦЕС 4] Нитка читання завершена\n");
    close(read_fd);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Використання: %s <read_fd>\n", argv[0]);
        exit(1);
    }
    
    // Отримуємо дескриптор для читання
    read_fd = atoi(argv[1]);
    
    printf("=== ПРОЦЕС 4 ЗАПУЩЕНО ===\n");
    printf("[ПРОЦЕС 4] Дескриптор читання: %d\n", read_fd);
    printf("[ПРОЦЕС 4] Роль: Прийом від процесу 3 → Виведення результату\n");
    
    // Встановлюємо обробники сигналів
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    // Створюємо нитку для читання
    pthread_t reader_tid;
    
    if (pthread_create(&reader_tid, NULL, reader_thread, NULL) != 0) {
        perror("[ПРОЦЕС 4] Помилка створення нитки читання");
        exit(1);
    }
    
    // Очікуємо завершення нитки
    pthread_join(reader_tid, NULL);
    
    printf("[ПРОЦЕС 4] Завершення роботи\n");
    
    return 0;
}