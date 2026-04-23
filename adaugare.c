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
        printf("open");
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
        printf("write");
        close(fd);
        return 1;
    }

    close(fd);
    printf("Added ID %d\n", r.id);

    log_action(d, role, user, "add");
    create_symlink(d);

    return 0;
}