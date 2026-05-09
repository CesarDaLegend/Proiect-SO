#include <signal.h>

#define PID_FILE ".monitor_pid"
// trimit SIGUSR1 la monitor cand se adauga un raport
// si scriu in log daca a mers sau nu
// citesc pid-ul din .monitor_pid
// returneaza -1 daca nu gaseste nimic

pid_t citeste_pid_monitor(){
    int fd = open(PID_FILE, O_RDONLY);
    if (fd == -1){
        // fisierul nu exista, monitorul probabil nu e pornit
        return -1;
    }

    char buf[32];
    int n = read(fd, buf, sizeof(buf) - 1);
    close(fd);

    if (n <= 0){
        return -1;
    }

    buf[n] = '\0';
    pid_t pid = (pid_t)atoi(buf);

    if (pid <= 0){
        return -1;
    }

    return pid;
}

// asta o apelez din add() dupa ce scriu raportul
// incearca sa trimita semnal la monitor si scrie in log ce s-a intamplat
void notifica_monitor(const char *district, const char *role, const char *user){
    char logfile[PATH];
    snprintf(logfile, PATH, "%s/logged_district", district);

    int fd = open(logfile, O_WRONLY | O_APPEND, 0644);
    if (fd == -1){
        return; // nu pot nici macar loga, ies
    }

    time_t t = time(NULL);
    char *ts = ctime(&t);
    ts[strcspn(ts, "\n")] = 0;

    char mesaj[512];
    pid_t pid = citeste_pid_monitor();

    if (pid == -1){
        // nu gasesc pid-ul, monitorul nu e pornit
        snprintf(mesaj, sizeof(mesaj),
            "%s role= %s user= %s, monitorul nu a putut fi notificat (nu gasesc .monitor_pid)\n",
            ts, role, user);
        write(fd, mesaj, strlen(mesaj));
        close(fd);
        return;
    }

    // incerc sa trimit SIGUSR1
    if (kill(pid, SIGUSR1) == -1){
        // a esuat, poate pid-ul e gresit sau procesul nu mai exista
        snprintf(mesaj, sizeof(mesaj),
            " %s role= %s user= %s,  monitorul nu a putut fi notificat (kill a esuat, pid=%d)\n",
            ts, role, user, pid);
    } 
    else{
        // a mers
        snprintf(mesaj, sizeof(mesaj),
            "%s role= %s user= %s, monitor notificat ok cu SIGUSR1 (pid=%d)\n",
            ts, role, user, pid);
    }

    write(fd, mesaj, strlen(mesaj));
    close(fd);
}