// gcc -Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast 15.c

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// для перечитывания переменной
volatile int g_last_signal;

// обработчик сигнала
void sig_handler(int signum) {
    g_last_signal = signum;
}

int main(void) {

    printf("kill -s SIGKILL %d\n", getpid());

    struct sigaction sigact_struct = {
        .sa_handler = sig_handler,
        .sa_flags = SA_RESTART
    };

    // обнулим все остальные поля
    sigemptyset(&sigact_struct.sa_mask);

    for(int signal = 1; signal <= NSIG; signal++) {
        if(sigaction(signal, &sigact_struct, NULL) == -1) {
            psignal(signal, NULL);
        }
    }

    siginfo_t  info;
    sigset_t   mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGTSTP);

    /* sigaddset(&mask, SIGILL);
    sigaddset(&mask, SIGABRT);
    sigaddset(&mask, SIGFPE);
    sigaddset(&mask, SIGSEGV);
    sigaddset(&mask, SIGPIPE);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGCONT);
    sigaddset(&mask, SIGSTOP);
    sigaddset(&mask, SIGTTIN);
    sigaddset(&mask, SIGTTOU); */

    // прога не падает потому что сигнал попал в обработчик
    while(1) {
        int signum = sigwaitinfo(&mask, &info);
        sig_handler(signum);
        printf(" last_signal = %d (%s)\n", g_last_signal, strsignal(g_last_signal));
    }
    return 0;
}