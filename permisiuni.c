#include "structura.c"

void permisiunea(mode_t m, char *p){
    //PENTRU USER
    p[0] = (m & S_IRUSR) ? 'r' : '-';
    p[1] = (m & S_IWUSR) ? 'w' : '-';
    p[2] = (m & S_IXUSR) ? 'x' : '-';

    //PENTRU GRUP
    p[3] = (m & S_IRGRP) ? 'r' : '-';
    p[4] = (m & S_IWGRP) ? 'w' : '-';
    p[5] = (m & S_IXGRP) ? 'x' : '-';

    //PENTRU RESTUL
    p[6] = (m & S_IROTH) ? 'r' : '-';
    p[7] = (m & S_IWOTH) ? 'w' : '-';
    p[8] = (m & S_IXOTH) ? 'x' : '-';

    //TERMINATOR
    p[9] = '\0';
}

int verifica_permisiunea(const char *cale, const char *rol, const char *operatie){
    struct stat st;
    if (stat(cale, &st) == -1){
        return 0;
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