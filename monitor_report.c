#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

#define PID_FILE ".monitor_pid"

// asta il setez pe 0 cand vine SIGINT ca sa ies din while
volatile int merge = 1;

void la_sigint(int sig){
    (void)sig; // ca sa nu zica compilatorul ca nu e folosit
    printf("\n[monitor] gata, inchid\n");
    merge = 0;
}

void la_sigusr1(int sig){
    (void)sig;
    // cineva a adaugat un raport nou
    printf("[monitor] s-a adaugat un raport nou!\n");
    fflush(stdout); // fara asta nu apare imediat in terminal
}

int main(){
    // salvez pid-ul meu in fisier ca sa-l gaseasca city_manager
    int fd = open(PID_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("nu am putut crea .monitor_pid");
        return 1;
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%d\n", getpid());
    write(fd, buf, strlen(buf));
    close(fd);

    printf("[monitor] am pornit, pid-ul meu e %d\n", getpid());
    printf("[monitor] astept semnale... ctrl+c ca sa opresti\n");
    fflush(stdout);

    // pun handlere pentru semnale
    // trebuie sigaction, nu signal() - asa zice cerinta
    struct sigaction sa_int, sa_usr1;

    sa_int.sa_handler = la_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_usr1.sa_handler = la_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    // stau in bucla si astept
    // pause() doarme pana vine un semnal, mai bine decat sleep in bucla
    while (merge){
        pause();
    }

    // sterg fisierul cu pid-ul la final
    unlink(PID_FILE);
    printf("[monitor] am sters .monitor_pid, pa!\n");

    return 0;
}