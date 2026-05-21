#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

#define PID_FILE ".monitor_pid"
#define MAX 256

// pipe-ul catre hub_mon. il primesc ca argument la pornire
// daca e -1, inseamna ca nimeni nu asteapta mesaje de la mine
int pipe_write = -1;

// asta o folosesc in handlerul de SIGINT ca sa ies din bucla
volatile int merge = 1;

// trimite un mesaj la hub_mon prin pipe
// format: "TIP:mesaj\n" - asa e mai usor de parsat la celalalt capat
void send_msg(const char *type, const char *mesaj){
    if (pipe_write == -1) return; // nu are cine sa citeasca
    
    char buffer[MAX];
    snprintf(buffer, sizeof(buffer), "%s:%s\n", type, mesaj);
    write(pipe_write, buffer, strlen(buffer));
}

// verifica daca mai e un monitor care ruleaza
// citeste pid-ul din .monitor_pid si face kill cu 0 sa vada daca mai exista
int check_existing_monitor(){
    int fd = open(PID_FILE, O_RDONLY);
    if (fd == -1){
        return 0; // nu exista fisier, sigur nu e alt monitor
    }

    char buf[32];
    int n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    
    if (n <= 0){
        return 0;
    }

    buf[n] = '\0';
    pid_t existing_pid = (pid_t)atoi(buf);
    if (existing_pid <= 0){
        return 0;
    }

    // kill cu 0 nu trimite semnal, doar verifica daca procesul exista
    if (kill(existing_pid, 0) == -1 && errno == ESRCH){
        return 0; // pid-ul e vechi, procesul nu mai e viu
    }
    
    return existing_pid; // am gasit un monitor deja pornit
}

// handler pentru Ctrl+C (SIGINT)
void la_sigint(int sig){
    (void)sig; // ca sa nu zica compilerul ca nu folosesc parametrul
    merge = 0; // ies din bucla din main
}

// handler pentru SIGUSR1 - il trimite city_manager cand adauga un raport nou
void la_sigusr1(int sig){
    (void)sig;
    send_msg("INFO", "s-a adaugat un raport nou!");
}

int main(int argc, char **argv){
    // daca am primit un file descriptor de pipe ca argument, il salvez
    if (argc >= 2){
        pipe_write = atoi(argv[1]);
    }
    
    // INTRU check: nu poti rula doi monitori in acelasi timp
    int existing = check_existing_monitor();
    if (existing > 0){
        char mesaj[100];
        snprintf(mesaj, sizeof(mesaj), "Eroare: monitor deja ruleaza cu pid=%d", existing);
        
        if(pipe_write != -1){
            send_msg("ERROR", mesaj);
        } 
        else{
            fprintf(stderr, "%s\n", mesaj);
        }
        return 1;
    }
    
    // scriu pid-ul meu in fisier ca sa ma gaseasca city_manager
    int fd = open(PID_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1){
        char mesaj[] = "Nu am putut crea .monitor_pid";
        if(pipe_write != -1){
            send_msg("ERROR", mesaj);
        } 
        else{
            perror(mesaj);
        }
        return 1;
    }
    
    char buf[32];
    snprintf(buf, sizeof(buf), "%d\n", getpid());
    write(fd, buf, strlen(buf));
    close(fd);
    
    // anunt ca am pornit (primesc hub_mon)
    char start_msg[100];
    snprintf(start_msg, sizeof(start_msg), "Monitor pornit cu pid=%d", getpid());
    send_msg("START", start_msg);
    
    // pun handler-ele pentru semnale
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
    
    // bucla principala - stau si astept semnale
    // pause() doarme pana vine un semnal, mai eficient decat sleep in bucla
    while (merge){
        pause();
    }
    
    // ajung aici cand vine SIGINT (Ctrl+C)
    unlink(PID_FILE); // sterg fisierul cu pid
    send_msg("STOP", "Monitor oprit normal");
    
    // inchid pipe-ul ca sa stie hub_mon ca s-a terminat
    if (pipe_write != -1){
        close(pipe_write);
    }
    
    return 0;
}