#include "structura.c"

#define MAX 100
//structura identica ca la city_manager

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "Folosirea scorer\n");
        return 1;
    }

    char *district = argv[1];
    char report_path[MAX];
    snprintf(report_path, sizeof(report_path), "%s/reports.dat", district);

    //deschid file-ul cu raporturi
    int fd = open(report_path, O_RDONLY);
    if(fd == -1){
        return 0;
    }

    //tin scorul pt fiecare inspector
    struct{
        char nume[MAX];
        int score;
    } scor[MAX];

    int nr_scor = 0;
    Raport r;

    //se citeste fiecare raport
    while(read(fd, &r, sizeof(r)) == sizeof(r)){
        //ma uit la inspector, daca e deja in lista
        int gasit = -1;

        for(int i = 0; i < nr_scor; i++){
            if(strcmp(scor[i].nume, r.inspector) == 0){
                gasit = i;
                break;
            }
        }

        //daca nu e, se adauga
        if(gasit == -1 && nr_scor < MAX){
            strcpy(scor[nr_scor].nume, r.inspector);
            scor[nr_scor].score = r.severitate;
            nr_scor++;
        }
        //daca exista deja, adun severitatea la scorul existent
        else if(gasit != -1){
            scor[gasit].score += r.severitate;
        }
    }
    close(fd);

    //afisarea rezultatelor
    printf("Workload pentru %s: \n", district);
    for(int i = 0; i < nr_scor; i++){
        printf("%s: %d\n", scor[i].nume, scor[i].score);
    }

    return 0;
}