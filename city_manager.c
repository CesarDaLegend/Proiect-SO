#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define MAX 100
#define PATH 200

typedef struct{
    int id;
    char inspector[MAX];
    double lat, lon;
    char categorie[MAX];
    int severitate;
    time_t timp;
    char descriere[MAX];
} Raport;

void permisiunea(mode_t m, char *p){
    p[0] = (m & S_IRUSR) ? 'r' : '-';
    p[1] = (m & S_IWUSR) ? 'w' : '-';
    p[2] = (m & S_IXUSR) ? 'x' : '-';

    p[3] = (m & S_IRGRP) ? 'r' : '-';
    p[4] = (m & S_IWGRP) ? 'w' : '-';
    p[5] = (m & S_IXGRP) ? 'x' : '-';
}

int verifica_permisiunea(const char *cale, const char *rol, const char *operatie){
    struct stat st;
    if (stat(cale, &st) == -1){
        return 1;
    }

    if (strcmp(rol, "manager") == 0){
        if (strcmp(operatie, "read") == 0){
            return (st.st_mode & S_IRUSR) ? 1:0;
        }
        if (strcmp(operatie, "write") == 0){
            return (st.st_mode & S_IWUSR) ? 1:0;
        }
    }

    if (strcmp(rol, "inspector") == 0){
        if (strcmp(operatie, "read") == 0){
            return (st.st_mode & S_IRGRP) ? 1:0;
        }
        if (strcmp(operatie, "write") == 0){
            return (st.st_mode & S_IWGRP) ? 1:0;
        }
    }

    return 0;
}

void init_district(const char *d){
    struct stat st;
    if (stat(d, &st) == -1){
        mkdir(d, 0750);
    }
    chmod(d, 0750);

    char f[PATH];
    snprintf(f, PATH, "%s/district.cfg", d);
    if (stat(f, &st) == -1){
        int fd = open(f, O_CREAT | O_WRONLY, 0640);
        if (fd != -1) {
            write(fd, "2", 1);
            close(fd);
        }
    }
    chmod(f, 0640);

    snprintf(f, PATH, "%s/logged_district", d);
    int fd = open(f, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd != -1) {
        close(fd);
    }
    chmod(f, 0644);
}

int next_id(const char *file){
    int fd = open(file, O_RDONLY);
    if (fd == -1){
        return 1;
    }

    Raport r;
    int id = 0;
    while (read(fd, &r, sizeof(r)) == sizeof(r)){
        id = r.id;
    }

    close(fd);
    return id + 1;
}

int add(const char *role, const char *user, const char *d){
    init_district(d);

    char f[PATH];
    snprintf(f, PATH, "%s/reports.dat", d);

    if (!verifica_permisiunea(f, role, "write")){
        printf("fara perm de a scrie\n");
        return 1;
    }

    int fd = open(f, O_CREAT | O_APPEND | O_RDWR, 0664);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    chmod(f, 0664);

    Raport r = {0};
    r.id = next_id(f);
    strncpy(r.inspector, user, MAX - 1);
    r.timp = time(NULL);

    printf("latitudine: ");
    scanf("%lf", &r.lat);

    printf("longitudine: ");
    scanf("%lf", &r.lon);

    printf("categorie: ");
    scanf("%99s", r.categorie);

    printf("severitate: ");
    scanf("%d", &r.severitate);
    getchar();

    printf("descriere: ");
    fgets(r.descriere, MAX, stdin);
    r.descriere[strcspn(r.descriere, "\n")] = 0;

    if (write(fd, &r, sizeof(r)) == -1){
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);
    printf("Added ID %d\n", r.id);
    return 0;
}

int list(const char *role, const char *d){
    char f[PATH];
    snprintf(f, PATH, "%s/reports.dat", d);

    if (!verifica_permisiunea(f, role, "read")){
        printf("No read permission\n");
        return 1;
    }

    int fd = open(f, O_RDONLY);
    if (fd == -1){
        printf("No file\n");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1){
        close(fd);
        return 1;
    }

    char p[7];
    permisiunea(st.st_mode, p);
    printf("Permisiunea: %s\n", p);
    printf("Size: %ld\n", (long)st.st_size);
    printf("Timpul: %s\n", ctime(&st.st_mtime));

    Raport r;
    while (read(fd, &r, sizeof(r)) == sizeof(r)){
        printf("%d | %s | %s | %d\n", r.id, r.inspector, r.categorie, r.severitate);
    }

    close(fd);
    return 0;
}

int view(const char *role, const char *d, int id){
    char f[PATH];
    snprintf(f, PATH, "%s/reports.dat", d);

    if (!verifica_permisiunea(f, role, "read")){
        printf("No read permission\n");
        return 1;
    }

    int fd = open(f, O_RDONLY);
    if (fd == -1){
        printf("Nu exista file-ul\n");
        return 1;
    }

    Raport r;
    while (read(fd, &r, sizeof(r)) == sizeof(r)){
        if (r.id == id) {
            printf("Inspector: %s\n", r.inspector);
            printf("Coordinatele: %.2f %.2f\n", r.lat, r.lon);
            printf("Categoria: %s\n", r.categorie);
            printf("Severitate: %d\n", r.severitate);
            printf("Timp: %s", ctime(&r.timp));
            printf("Descriere: %s\n", r.descriere);
            close(fd);
            return 0;
        }
    }

    printf("Nu exista\n");
    close(fd);
    return 1;
}

int delete_report(const char *role, const char *d, int id){
    if (strcmp(role, "manager") != 0){
        printf("Only manager\n");
        return 1;
    }

    char f[PATH];
    snprintf(f, PATH, "%s/reports.dat", d);

    int fd = open(f, O_RDWR);
    if (fd == -1) {
        return 1;
    }

    struct stat st;
    fstat(fd, &st);
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
        printf("Not found\n");
        free(v);
        close(fd);
        return 1;
    }

    for (int i = pos; i < n - 1; i++){
        lseek(fd, i * sizeof(Raport), SEEK_SET);
        write(fd, &v[i + 1], sizeof(Raport));
    }

    if (ftruncate(fd, (n - 1) * sizeof(Raport)) == -1){
        printf("Eroare la truncare\n");
        free(v);
        close(fd);
        return 1;
    }

    free(v);
    close(fd);
    printf("Deleted\n");
    return 0;
}

int update(const char *role, const char *d, int val){
    char f[PATH];
    snprintf(f, PATH, "%s/district.cfg", d);

    if (strcmp(role, "manager") != 0){
        printf("Only manager\n");
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
    printf("Updated\n");
    return 0;
}

int main(int argc, char **argv) {
    char *role = NULL, *user = NULL, *cmd = NULL, *d = NULL;
    int id = -1, val = -1;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--role") == 0){
            role = argv[++i];
        } else if (strcmp(argv[i], "--user") == 0){
            user = argv[++i];
        } else if (strcmp(argv[i], "--add") == 0){
            cmd = "add";
            d = argv[++i];
        } else if (strcmp(argv[i], "--list") == 0){
            cmd = "list";
            d = argv[++i];
        } else if (strcmp(argv[i], "--view") == 0){
            cmd = "view";
            d = argv[++i];
            id = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--remove_report") == 0){
            cmd = "rm";
            d = argv[++i];
            id = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--update_threshold") == 0){
            cmd = "up";
            d = argv[++i];
            val = atoi(argv[++i]);
        }
    }

    if (role == NULL || user == NULL || cmd == NULL){
        printf("Bad args\n");
        return 1;
    }

    if (strcmp(cmd, "add") == 0){
        return add(role, user, d);
    }
    if (strcmp(cmd, "list") == 0){
        return list(role, d);
    }
    if (strcmp(cmd, "view") == 0){
        return view(role, d, id);
    }
    if (strcmp(cmd, "rm") == 0){
        return delete_report(role, d, id);
    }
    if (strcmp(cmd, "up") == 0){
        return update(role, d, val);
    }

    return 0;
}