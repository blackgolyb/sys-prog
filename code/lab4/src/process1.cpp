#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 256

// Глобальні змінні
int write_fd;
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
    printf("[ПРОЦЕС 1] Введіть рядки для відправки (Ctrl+D або 'exit' для завершення):\n");
    
    while (running) {
        printf("1> ");
        fflush(stdout);
        
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            // EOF (Ctrl+D) або помилка читання
            printf("\n[ПРОЦЕС 1] Завершення введення\n");
            break;
        }
        
        // Перевірка на команду виходу
        if (strncmp(buffer, "exit", 4) == 0 || strncmp(buffer, "quit", 4) == 0) {
            printf("[ПРОЦЕС 1] Команда завершення отримана\n");
            break;
        }
        
        // Видаляємо зайвий символ нового рядка, якщо він є
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        
        if (len == 0) {
            continue; // Пропускаємо порожні рядки
        }
        
        // Відправляємо дані в канал
        ssize_t bytes_written = write(write_fd, buffer, len + 1); // +1 для '\0'
        
        if (bytes_written < 0) {
            perror("[ПРОЦЕС 1] Помилка запису в канал");
            break;
        }
        
        printf("[ПРОЦЕС 1] 1→2: передано \"%s\" (%zd байт)\n", buffer, bytes_written);
    }
    
    printf("[ПРОЦЕС 1] Нитка запису завершена\n");
    close(write_fd);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Використання: %s <write_fd>\n", argv[0]);
        exit(1);
    }
    
    // Отримуємо дескриптор для запису
    write_fd = atoi(argv[1]);
    
    printf("=== ПРОЦЕС 1 ЗАПУЩЕНО ===\n");
    printf("[ПРОЦЕС 1] Дескриптор запису: %d\n", write_fd);
    printf("[ПРОЦЕС 1] Роль: Читання з клавіатури → Відправка до процесу 2\n");
    
    // Встановлюємо обробники сигналів
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    // Створюємо нитку для запису
    pthread_t writer_tid;
    
    if (pthread_create(&writer_tid, NULL, writer_thread, NULL) != 0) {
        perror("[ПРОЦЕС 1] Помилка створення нитки запису");
        exit(1);
    }
    
    // Очікуємо завершення нитки
    pthread_join(writer_tid, NULL);
    
    printf("[ПРОЦЕС 1] Завершення роботи\n");
    
    return 0;
}