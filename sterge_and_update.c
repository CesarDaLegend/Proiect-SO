#include <stdlib.h>

int delete_report(const char *role, const char *user, const char *d, int id){
    if (strcmp(role, "manager") != 0){
        printf("doar managerul\n");
        return 1;
    }

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

int update(const char *role, const char *user, const char *d, int val){
    char f[PATH];
    snprintf(f, PATH, "%s/district.cfg", d);

    if (strcmp(role, "manager") != 0){
        printf("doar managerul\n");
        return 1;
    }

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