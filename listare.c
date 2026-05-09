#include "structura.c"
#include "permisiuni.c"
#include "ai.c"

int list(const char *role, const char *user, const char *d){
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

    char p[10];
    permisiunea(st.st_mode, p);
    printf("Permisiunea: %s\n", p);
    printf("Size: %ld\n", (long)st.st_size);
    printf("Timpul: %s\n", ctime(&st.st_mtime));

    Raport r;
    while (read(fd, &r, sizeof(r)) == sizeof(r)){
        printf("%d | %s | %s | %d\n", r.id, r.inspector, r.categorie, r.severitate);
    }

    close(fd);

    log_action(d, role, user, "list");
    create_symlink(d);

    /*afiseaza toate rapoartele din district */
    return 0;
}

int view(const char *role, const char *d, int id){
    /*cauta un raport dupa id si afiseaza toate detaliile lui*/
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