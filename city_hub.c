#include "structura.c"  

#define MAX_DISTRICTS 10
#define MAX_LINE 256

void start_monitor(){
    int pipefd[2];
    if(pipe(pipefd) == -1){
        perror("pipe");
        return;
    }
    
    pid_t hub_mon = fork();
    if(hub_mon == -1){
        perror("fork");
        return;
    }
    
    if(hub_mon == 0){
        close(pipefd[0]);
        
        pid_t mon = fork();
        if(mon == -1) exit(1);
        
        if(mon == 0){
            char pfd[16];
            snprintf(pfd, sizeof(pfd), "%d", pipefd[1]);
            execlp("./monitor_reports", "monitor_reports", pfd, NULL);
            perror("execlp");
            exit(1);
        }
        
        int status;
        waitpid(mon, &status, 0);
        
        char msg[MAX_LINE];
        if(WIFEXITED(status))
            snprintf(msg, sizeof(msg), "STOP:Monitor terminat cod=%d", WEXITSTATUS(status));
        else
            snprintf(msg, sizeof(msg), "STOP:Monitor terminat anormal");
        write(pipefd[1], msg, strlen(msg));
        close(pipefd[1]);
        exit(0);
    }
    
    close(pipefd[1]);
    printf("[city_hub] Monitor pornit (hub_mon=%d)\n", hub_mon);
    printf("Astept mesaje...\n");
    
    char buf[MAX_LINE];
    ssize_t n;
    while((n = read(pipefd[0], buf, sizeof(buf)-1)) > 0){
        buf[n] = '\0';
        char *newline = strchr(buf, '\n');
        if(newline) *newline = '\0';
        
        char *colon = strchr(buf, ':');
        if(colon){
            *colon = '\0';
            if(strcmp(buf, "START") == 0){
                printf("[MONITOR] A INCEPUT: %s\n", colon+1);
            }
            else if(strcmp(buf, "INFO") == 0){
                printf("[MONITOR] Info: %s\n", colon+1);
            }
            else if(strcmp(buf, "ERROR") == 0){
                printf("[MONITOR] EROARE: %s\n", colon+1);
            }
            else if(strcmp(buf, "STOP") == 0){
                printf("[MONITOR] S-A OPRI T: %s\n", colon+1);
                break;
            }
        }
        fflush(stdout);
    }
    close(pipefd[0]);
    waitpid(hub_mon, NULL, 0);
    printf("hub_mon terminat\n");
}

//score, calculeaza
void run_scorer(const char *d){
    int p[2];
    if(pipe(p) == -1){
        perror("pipe");
        return;
    }
    
    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        return;
    }
    
    if(pid == 0){
        close(p[0]);
        dup2(p[1], STDOUT_FILENO);
        close(p[1]);
        execlp("./scorer", "scorer", d, NULL);
        perror("execlp");
        exit(1);
    }
    
    close(p[1]);
    printf("\n=== Scoruri %s ===\n", d);
    
    char buf[MAX_LINE];
    ssize_t n;
    while((n = read(p[0], buf, sizeof(buf)-1)) > 0){
        buf[n] = '\0';
        printf("%s", buf);
    }
    close(p[0]);
    waitpid(pid, NULL, 0);
}

void calculate_scores(int cnt, char **d){
    printf("\n=== CALCUL WORKLOAD ===\n");
    for(int i=0; i<cnt; i++){
        char path[200];
        snprintf(path, sizeof(path), "%s/reports.dat", d[i]);
        if(access(path, F_OK) == -1){
            printf("District %s nu exista\n", d[i]);
            continue;
        }
        run_scorer(d[i]);
    }
    printf("\n=== GATA ===\n");
}

int main(){
    char cmd[200];
    printf("===> CITY HUB ===\n");
    printf("start_monitor\ncalculate_scores <d1 d2...>\nexit\n\n");
    
    while(1){
        printf("city_hub> ");
        fflush(stdout);
        if(fgets(cmd, sizeof(cmd), stdin) == NULL){
            break;
        }
        cmd[strcspn(cmd, "\n")] = '\0';
        
        if(strcmp(cmd, "exit") == 0){
            printf("La revedere!\n");
            break;
        }
        if(strcmp(cmd, "start_monitor") == 0){
            start_monitor();
            continue;
        }
        if(strncmp(cmd, "calculate_scores", 16) == 0){
            char *args[MAX_DISTRICTS];
            int cnt = 0;
            char *tok = strtok(cmd, " ");
            tok = strtok(NULL, " ");
            while(tok && cnt < MAX_DISTRICTS){
                args[cnt++] = tok;
                tok = strtok(NULL, " ");
            }
            if(cnt > 0) calculate_scores(cnt, args);
            else{
                printf("Dai macar un district\n");
            }
            continue;
        }
        printf("Comanda gresita\n");
    }
    return 0;
}