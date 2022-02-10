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

    // зачем заполняем sa_mask всеми сигналами?
    sigfillset(&sigact_struct.sa_mask);

    // что тут происходит?
    for(int signal = 1; signal <= NSIG; signal++) {
        if(sigaction(signal, &sigact_struct, NULL) == -1) {
            psignal(signal, NULL);
        }
    }

    siginfo_t  info;
    sigset_t mask;

    sigfillset(&mask);

    // что тут происходит?
    while(1) {
        int signum = sigwaitinfo(&mask, &info);
        printf(" last_signal = %d (%s)\n", signum, strsignal(signum));
    }
    return 0;
}