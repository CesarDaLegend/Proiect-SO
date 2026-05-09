#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX 100
#define PATH 200

typedef struct{
    int id;
    char inspector[MAX];
    double lat;
    double lon;
    char categorie[MAX];
    int severitate;
    time_t timp;
    char descriere[MAX];
} Raport;