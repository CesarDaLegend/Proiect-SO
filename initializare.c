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