#include "structura.c"
#include "permisiuni.c"
#include "initializare.c"
#include "ai.c"
#include "adaugare.c"
#include "listare.c"
#include "sterge_and_update.c"

int main(int argc, char **argv) {
    char *role = NULL;
    char *user = NULL; 
    char *cmd = NULL;
    char *d = NULL;

    int id = -1; 
    int val = -1;
    int filter_start = -1;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--role") == 0){
            role = argv[++i];
        }
        if (strcmp(argv[i], "--user") == 0){
            user = argv[++i];
        }
        if (strcmp(argv[i], "--add") == 0){
            cmd = "add";
            d = argv[++i];
        }
        if (strcmp(argv[i], "--list") == 0){
            cmd = "list";
            d = argv[++i];
        }
        if (strcmp(argv[i], "--view") == 0){
            cmd = "view";
            d = argv[++i];
            id = atoi(argv[++i]);
        }
        if (strcmp(argv[i], "--remove_report") == 0){
            cmd = "rm";
            d = argv[++i];
            id = atoi(argv[++i]);
        }
        if (strcmp(argv[i], "--update_threshold") == 0){
            cmd = "up";
            d = argv[++i];
            val = atoi(argv[++i]);
        }
        if(strcmp(argv[i], "--filter") == 0){
            cmd = "filter";
            d = argv[++i];
            filter_start = i+1;
            break;
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
        return list(role, user, d);
    }
    if (strcmp(cmd, "view") == 0){
        return view(role, d, id);
    }
    if (strcmp(cmd, "rm") == 0){
        return delete_report(role, user, d, id);
    }
    if (strcmp(cmd, "up") == 0){
        return update(role, user, d, val);
    }
    if (strcmp(cmd, "filter") == 0) {
        return filter(role, d, argc, argv, filter_start);
    }

    return 0;
}