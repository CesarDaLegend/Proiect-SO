#include "structura.c"
#include "permisiuni.c"

int parse_condition(const char *input, char *field, char *op, char *value){
    char tmp[MAX];
    strncpy(tmp, input, MAX - 1);
    tmp[MAX - 1] = '\0';

    char *p1 = strchr(tmp, ':');
    if (p1 == NULL){
        return 0;
    }
    *p1 = '\0';
    strcpy(field, tmp);

    char *p2 = strchr(p1 + 1, ':');
    if (p2 == NULL){
        return 0;
    }
    *p2 = '\0';
    strcpy(op, p1 + 1);
    strcpy(value, p2 + 1);

    return 1;
}

int match_condition(Raport *r, const char *field, const char *op, const char *value){
    if (strcmp(field, "severity") == 0){
        int val = atoi(value);
        if (strcmp(op, "==") == 0) return r->severitate == val;
        if (strcmp(op, "!=") == 0) return r->severitate != val;
        if (strcmp(op, "<")  == 0) return r->severitate <  val;
        if (strcmp(op, "<=") == 0) return r->severitate <= val;
        if (strcmp(op, ">")  == 0) return r->severitate >  val;
        if (strcmp(op, ">=") == 0) return r->severitate >= val;
    }

    if (strcmp(field, "category") == 0){
        int cmp = strcmp(r->categorie, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    }

    if (strcmp(field, "inspector") == 0){
        int cmp = strcmp(r->inspector, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    }

    if (strcmp(field, "timestamp") == 0){
        time_t val = (time_t)atol(value);
        if (strcmp(op, "==") == 0) return r->timp == val;
        if (strcmp(op, "!=") == 0) return r->timp != val;
        if (strcmp(op, "<")  == 0) return r->timp <  val;
        if (strcmp(op, "<=") == 0) return r->timp <= val;
        if (strcmp(op, ">")  == 0) return r->timp >  val;
        if (strcmp(op, ">=") == 0) return r->timp >= val;
    }

    return 0;
}

int filter(const char *role, const char *d, int argc, char **argv, int start){
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

    char fields[10][MAX], ops[10][MAX], values[10][MAX];
    int nr_cond = 0;

    for (int i = start; i < argc; i++){
        if (parse_condition(argv[i], fields[nr_cond], ops[nr_cond], values[nr_cond])){
            nr_cond++;
        }
    }

    Raport r;
    while (read(fd, &r, sizeof(r)) == sizeof(r)){
        int ok = 1;
        for (int i = 0; i < nr_cond; i++){
            if (!match_condition(&r, fields[i], ops[i], values[i])){
                ok = 0;
                break;
            }
        }
        if (ok){
            char *timestr = ctime(&r.timp);
            timestr[strcspn(timestr, "\n")] = 0;
            printf("%d | %s | %s | %d | %s\n", r.id, r.inspector, r.categorie, r.severitate, timestr);
        }
    }

    close(fd);
    return 0;
}

void create_symlink(const char *d){
    char target[PATH], link[PATH];
    snprintf(target, PATH, "%s/reports.dat", d);
    snprintf(link, PATH, "active_reports-%s", d);

    struct stat st;
    if (lstat(link, &st) == 0){
        unlink(link);
    }

    if (symlink(target, link) == -1){
        printf("Eroare la crearea symlink-ului\n");
    }
}

void check_symlink(const char *d){
    char link[PATH];
    snprintf(link, PATH, "active_reports-%s", d);

    struct stat lst, st;
    if (lstat(link, &lst) == -1){
        return;
    }

    if (stat(link, &st) == -1){
        printf("Warning: dangling symlink detectat: %s\n", link);
    }
}

void log_action(const char *d, const char *role, const char *user, const char *action){
    char f[PATH];
    snprintf(f, PATH, "%s/logged_district", d);

    if (!verifica_permisiunea(f, role, "write")){
        printf("No write permission pentru log\n");
        return;
    }

    int fd = open(f, O_WRONLY | O_APPEND, 0644);
    if (fd == -1){
        return;
    }

    time_t t = time(NULL);
    char *ts = ctime(&t);
    ts[strcspn(ts, "\n")] = 0;

    char buf[512];
    snprintf(buf, sizeof(buf), "[%s] role=%s user=%s action=%s\n", ts, role, user, action);
    write(fd, buf, strlen(buf));
    close(fd);
}