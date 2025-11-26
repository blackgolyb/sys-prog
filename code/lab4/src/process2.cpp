#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 256

// Глобальні змінні
int read_fd;
int write_fd;
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
    
    while (running) {
        ssize_t bytes_read = read(read_fd, buffer, BUFFER_SIZE);
        
        if (bytes_read < 0) {
            if (running) {
                perror("[ПРОЦЕС 2] Помилка читання з каналу");
            }
            break;
        }
        
        if (bytes_read == 0) {
            // EOF - процес 1 закрив канал
            printf("[ПРОЦЕС 2] Процес 1 завершив передачу даних\n");
            break;
        }
        
        printf("[ПРОЦЕС 2] 1→2: прийнято \"%s\" (%zd байт)\n", buffer, bytes_read);
        
        // Автоматичне переспрямування до процесу 3
        ssize_t bytes_written = write(write_fd, buffer, bytes_read);
        
        if (bytes_written < 0) {
            perror("[ПРОЦЕС 2] Помилка запису в канал");
            break;
        }
        
        printf("[ПРОЦЕС 2] 2→3: переспрямовано \"%s\" (%zd байт)\n", buffer, bytes_written);
    }
    
    printf("[ПРОЦЕС 2] Нитка читання завершена\n");
    close(read_fd);
    close(write_fd);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Використання: %s <read_fd> <write_fd>\n", argv[0]);
        exit(1);
    }
    
    // Отримуємо дескриптори
    read_fd = atoi(argv[1]);
    write_fd = atoi(argv[2]);
    
    printf("=== ПРОЦЕС 2 ЗАПУЩЕНО ===\n");
    printf("[ПРОЦЕС 2] Дескриптор читання: %d\n", read_fd);
    printf("[ПРОЦЕС 2] Дескриптор запису: %d\n", write_fd);
    printf("[ПРОЦЕС 2] Роль: Прийом від процесу 1 → Переспрямування до процесу 3\n");
    
    // Встановлюємо обробники сигналів
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    // Створюємо нитку для читання та автоматичного переспрямування
    pthread_t reader_tid;
    
    if (pthread_create(&reader_tid, NULL, reader_thread, NULL) != 0) {
        perror("[ПРОЦЕС 2] Помилка створення нитки читання");
        exit(1);
    }
    
    // Очікуємо завершення нитки
    pthread_join(reader_tid, NULL);
    
    printf("[ПРОЦЕС 2] Завершення роботи\n");
    
    return 0;
}