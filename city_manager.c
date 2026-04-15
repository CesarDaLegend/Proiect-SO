#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>

//layout-ul
typedef struct{
    int id;
    char inspector[128];
    float latitudine;
    float longitudine;
    char category[64];
    int severity;
    time_t timestamp;
    char descreption[256];
} Report;

//argumente globale
static char *role = NULL;
static char *user = NULL;
static char *comanda = NULL; //add, list, view, remove...
static char *district = NULL; //ferentari, rahova...


//verific daca rolul respectiv are voie sa scrie in logged district
void rol(mode_t role, char *out){
    out[0] = (role & S_IRUSR) ? 'r':'-';
    out[1] = (role & S_IWUSR) ? 'w':'-';
    out[2] = (role & S_IXUSR) ? 'x':'-';

    out[3] = (role & S_IRGRP) ? 'r':'-';
    out[4] = (role & S_IWGRP) ? 'w':'-';
    out[5] = (role & S_IXGRP) ? 'x':'-';
}


//managerul are full acces. poate scrie cand vrea, unde vrea

//in logged_district
int manager_scrie_ld(){
    return strcmp(role, "manager") == 0;
}
//in district.cfg
int manager_scrie_dc(){
    return strcmp(role, "manager") == 0;
}

//ma uit daca rolul respectiv poate citi din district cfg
int citeste(const char *cfg){
    struct stat st;
    
    if(stat(cfg, &st) < 0){
        return 0;
    }
    if(stat(role, "manager") == 0){
        return (st.st_mode & S_IRUSR) ? 1:0;
    }
    else{
        return (st.st_mode & S_IRGRP) ? 1;0;
    }
}

//scriere in log
void log(const char *comuna, const char *action){
    if(!manager_scrie_ld){  
        //trecem peste, fara nici un pacar de warning
        return;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/logged-district", comuna);
}