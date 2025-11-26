#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1

// Глобальні змінні для керування процесами
pid_t pids[4];
int num_processes = 0;

void cleanup_and_exit(int sig) {
    printf("\n[MANAGER] Отримано сигнал завершення. Зупинка всіх процесів...\n");
    
    // Відправляємо SIGTERM всім дочірнім процесам
    for (int i = 0; i < num_processes; i++) {
        if (pids[i] > 0) {
            kill(pids[i], SIGTERM);
        }
    }
    
    // Чекаємо завершення всіх процесів
    for (int i = 0; i < num_processes; i++) {
        if (pids[i] > 0) {
            waitpid(pids[i], NULL, 0);
        }
    }
    
    printf("[MANAGER] Всі процеси завершено.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    // Канали для зв'язку: 1→2, 2→3, 3→4
    int pipe_1_2[2]; // Канал від процесу 1 до процесу 2
    int pipe_2_3[2]; // Канал від процесу 2 до процесу 3
    int pipe_3_4[2]; // Канал від процесу 3 до процесу 4
    
    printf("=== MANAGER: Запуск системи обміну даними ===\n");
    printf("Схема передачі: 1 → 2 → 3 → 4 (автоматичне переспрямування)\n");
    printf("Для завершення натисніть Ctrl+C\n\n");
    
    // Встановлюємо обробник сигналу для коректного завершення
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    // Створюємо канали
    if (pipe(pipe_1_2) < 0) {
        perror("[MANAGER] Помилка створення pipe_1_2");
        exit(1);
    }
    
    if (pipe(pipe_2_3) < 0) {
        perror("[MANAGER] Помилка створення pipe_2_3");
        exit(1);
    }
    
    if (pipe(pipe_3_4) < 0) {
        perror("[MANAGER] Помилка створення pipe_3_4");
        exit(1);
    }
    
    printf("[MANAGER] Всі канали створено успішно\n");
    
    // Буфери для передачі дескрипторів як аргументів
    char read_fd_str[16];
    char write_fd_str[16];
    
    // === Запуск процесу 1 (читає з stdin, пише в pipe_1_2) ===
    printf("[MANAGER] Запуск процесу 1...\n");
    pids[0] = fork();
    if (pids[0] < 0) {
        perror("[MANAGER] Помилка fork для процесу 1");
        exit(1);
    }
    
    if (pids[0] == 0) {
        // Дочірній процес 1
        close(pipe_1_2[READ_END]);  // Не використовується
        close(pipe_2_3[READ_END]);
        close(pipe_2_3[WRITE_END]);
        close(pipe_3_4[READ_END]);
        close(pipe_3_4[WRITE_END]);
        
        sprintf(write_fd_str, "%d", pipe_1_2[WRITE_END]);
        
        execl("./build/process1", "process1", write_fd_str, NULL);
        perror("[MANAGER] Помилка execl для процесу 1");
        exit(1);
    }
    num_processes++;
    
    // === Запуск процесу 2 (читає з pipe_1_2, пише в pipe_2_3) ===
    printf("[MANAGER] Запуск процесу 2...\n");
    pids[1] = fork();
    if (pids[1] < 0) {
        perror("[MANAGER] Помилка fork для процесу 2");
        exit(1);
    }
    
    if (pids[1] == 0) {
        // Дочірній процес 2
        close(pipe_1_2[WRITE_END]); // Не використовується
        close(pipe_2_3[READ_END]);  // Не використовується
        close(pipe_3_4[READ_END]);
        close(pipe_3_4[WRITE_END]);
        
        sprintf(read_fd_str, "%d", pipe_1_2[READ_END]);
        sprintf(write_fd_str, "%d", pipe_2_3[WRITE_END]);
        
        execl("./build/process2", "process2", read_fd_str, write_fd_str, NULL);
        perror("[MANAGER] Помилка execl для процесу 2");
        exit(1);
    }
    num_processes++;
    
    // === Запуск процесу 3 (читає з pipe_2_3, пише в pipe_3_4) ===
    printf("[MANAGER] Запуск процесу 3...\n");
    pids[2] = fork();
    if (pids[2] < 0) {
        perror("[MANAGER] Помилка fork для процесу 3");
        exit(1);
    }
    
    if (pids[2] == 0) {
        // Дочірній процес 3
        close(pipe_1_2[READ_END]);
        close(pipe_1_2[WRITE_END]);
        close(pipe_2_3[WRITE_END]); // Не використовується
        close(pipe_3_4[READ_END]);  // Не використовується
        
        sprintf(read_fd_str, "%d", pipe_2_3[READ_END]);
        sprintf(write_fd_str, "%d", pipe_3_4[WRITE_END]);
        
        execl("./build/process3", "process3", read_fd_str, write_fd_str, NULL);
        perror("[MANAGER] Помилка execl для процесу 3");
        exit(1);
    }
    num_processes++;
    
    // === Запуск процесу 4 (читає з pipe_3_4, виводить на stdout) ===
    printf("[MANAGER] Запуск процесу 4...\n");
    pids[3] = fork();
    if (pids[3] < 0) {
        perror("[MANAGER] Помилка fork для процесу 4");
        exit(1);
    }
    
    if (pids[3] == 0) {
        // Дочірній процес 4
        close(pipe_1_2[READ_END]);
        close(pipe_1_2[WRITE_END]);
        close(pipe_2_3[READ_END]);
        close(pipe_2_3[WRITE_END]);
        close(pipe_3_4[WRITE_END]); // Не використовується
        
        sprintf(read_fd_str, "%d", pipe_3_4[READ_END]);
        
        execl("./build/process4", "process4", read_fd_str, NULL);
        perror("[MANAGER] Помилка execl для процесу 4");
        exit(1);
    }
    num_processes++;
    
    // Закриваємо всі дескриптори в батьківському процесі
    close(pipe_1_2[READ_END]);
    close(pipe_1_2[WRITE_END]);
    close(pipe_2_3[READ_END]);
    close(pipe_2_3[WRITE_END]);
    close(pipe_3_4[READ_END]);
    close(pipe_3_4[WRITE_END]);
    
    printf("[MANAGER] Всі процеси запущено. Система працює...\n\n");
    
    // Очікуємо завершення всіх дочірніх процесів
    for (int i = 0; i < num_processes; i++) {
        int status;
        pid_t finished_pid = wait(&status);
        printf("[MANAGER] Процес з PID %d завершено\n", finished_pid);
    }
    
    printf("\n[MANAGER] Всі процеси завершили роботу. Вихід.\n");
    
    return 0;
}