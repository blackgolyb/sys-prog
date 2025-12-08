#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t reminder_count = 0;
volatile sig_atomic_t should_exit = 0;

const char *messages[] = {
    "Нагадування №1: Вже пізно, треба спати! Але філософ продовжує читати...\n",
    "Нагадування №2: Вже пізно, треба спати! Але філософ продовжує читати...\n",
    "Нагадування №3: Вже пізно, треба спати! Але філософ продовжує читати...\n",
    "Нагадування №4: Вже пізно, треба спати! Але філософ продовжує читати...\n",
    "Нагадування №5: Вже пізно, треба спати! Але філософ продовжує читати...\n"
};

void sigusr1_handler(int sig __attribute__((unused))) {
    reminder_count++;

    if (reminder_count <= 5) {
        write(STDOUT_FILENO, messages[reminder_count - 1], strlen(messages[reminder_count - 1]));
    }

    if (reminder_count >= 5) {
        should_exit = 1;
    }
}

void sigint_handler(int sig __attribute__((unused))) {
    const char *msg = "\nРаптове переривання! Філософ заснув сам.\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    exit(0);
}

int main(void) {
    struct sigaction sa_usr1;
    struct sigaction sa_int;

    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(1);
    }

    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(1);
    }

    printf("Філософ починає вивчати праці...\n");
    printf("PID процесу: %d\n", getpid());
    printf("Надішліть SIGUSR1 для нагадування (kill -SIGUSR1 %d)\n", getpid());
    printf("Натисніть Ctrl+C для негайного переривання\n\n");

    while (!should_exit) {
        pause();
    }

    printf("Філософ погоджується: Досить на сьогодні! Час спати.\n");

    return 0;
}
