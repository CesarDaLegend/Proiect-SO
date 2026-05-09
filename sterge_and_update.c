#include "structura.c"
#include "permisiuni.c"
#include "ai.c"

int delete_report(const char *role, const char *user, const char *d, int id){
    /*sterge un raport dupa id */
    if (strcmp(role, "manager") != 0){
        printf("doar managerul\n");
        return 1;
    }
    /*DOAR MANAGERUL POATE SA STEARGA RAPORTUL DORIT!!!*/
    
    char f[PATH];
    snprintf(f, PATH, "%s/reports.dat", d);

    int fd = open(f, O_RDWR);
    if (fd == -1){
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1){
        close(fd);
        return 1;
    }

    int n = st.st_size / sizeof(Raport);

    Raport *v = malloc(n * sizeof(Raport));
    if (v == NULL) {
        close(fd);
        return 1;
    }
    read(fd, v, n * sizeof(Raport));

    int pos = -1;
    for (int i = 0; i < n; i++){
        if (v[i].id == id){
            pos = i;
            break;
        }
    }

    if (pos == -1){
        printf("nu exista\n");
        free(v);
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET);
    for (int i = 0; i < n; i++){
        if (i != pos){
            write(fd, &v[i], sizeof(Raport));
        }
    }

    if (ftruncate(fd, (n - 1) * sizeof(Raport)) == -1){
        printf("Eroare la truncare\n");
        free(v);
        close(fd);
        return 1;
    }

    free(v);
    close(fd);
    printf("e sters, sorry\n");

    log_action(d, role, user, "remove_report");
    create_symlink(d);
    
    return 0;
}


// sterg tot districtul: directorul cu tot ce e in el si symlink-ul
// doar managerul poate face asta
// fac fork + execvp ca sa apelez rm -rf
int remove_district(const char *role, const char *d){
    // doar managerul sterge districte
    if (strcmp(role, "manager") != 0){
        printf("doar managerul poate sterge un district\n");
        return 1;
    }

    // verific ca districtul chiar exista inainte sa fac orice
    struct stat st;
    if (stat(d, &st) == -1){
        printf("districtul '%s' nu exista\n", d);
        return 1;
    }

    printf("sterg districtul '%s'...\n", d);

    // creez un proces copil care va rula "rm -rf <district>"
    pid_t pid = fork();

    if (pid == -1){
        perror("fork a esuat");
        return 1;
    }

    if (pid == 0){
        // sunt in procesul copil acum
        // construiesc argumentele pentru rm si le trimit
        char *args[] = {"rm", "-rf", (char *)d, NULL};
        execvp("rm", args);

        // daca ajung aici inseamna ca execvp a esuat
        perror("execvp a esuat");
        exit(1);
    }

    // parintele asteapta sa termine copilul
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0){
        printf("directorul '%s' a fost sters\n", d);
    } 
    else{
        printf("ceva nu merge la stergerea lui '%s'\n", d);
        return 1;
    }

    // sterg si symlink-ul active_reports-<district>
    char link[PATH];
    snprintf(link, PATH, "active_reports-%s", d);

    // incerc sa-l sterg, daca nu exista nu e o problema
    if (unlink(link) == 0){
        printf("am sters si symlink-ul '%s'\n", link);
    }

    return 0;
}

int update(const char *role, const char *user, const char *d, int val){
    /* schimba pragul de severitate din "district.cfg"*/
    char f[PATH];
    snprintf(f, PATH, "%s/district.cfg", d);

    if (strcmp(role, "manager") != 0){
        printf("doar managerul\n");
        return 1;
    }
    /*DOAR MANAGERUL POATE FACE*/

    if (!verifica_permisiunea(f, role, "read")){
        printf("No read permission\n");
        return 1;
    }

    if (!verifica_permisiunea(f, role, "write")){
        printf("No write permission\n");
        return 1;
    }

    struct stat st;
    if (stat(f, &st) == -1){
        return 1;
    }

    if ((st.st_mode & 0777) != 0640){
        printf("Wrong permissions\n");
        return 1;
    }

    int fd = open(f, O_WRONLY | O_TRUNC, 0640);
    if (fd == -1){
        return 1;
    }

    char buf[10];
    sprintf(buf, "%d", val);
    write(fd, buf, strlen(buf));
    close(fd);
    chmod(f, 0640);

    log_action(d, role, user, "update_threshold");

    printf("Updated\n");
    return 0;
}