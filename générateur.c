#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char cle_api[256];
    char id_logement[256];
    char chemin_logs[256];
    char chemin_donnee[256];
} Configuration;

char *creationLogs() {

    int day, mois, an;
    time_t now = time(NULL);
    printf("ok 1 - logs\n");

    struct tm *local = localtime(&now);
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

    char *filename = malloc(256);
    printf("ok 2 - logs\n");
    sprintf(filename, "logs/logs-%02d_%02d_%d.log", day, mois, an);
    printf("ok 3 - logs\n");
    fopen(filename, "a");
    printf("ok 4 - logs\n");

    return filename;
}

char *creationJson() {

    int day, mois, an, h, min, s;
    time_t now = time(NULL);
    printf("ok 1 - json\n");

    struct tm *local = localtime(&now);
        h = local->tm_hour;
        min = local->tm_min;
        s = local->tm_sec;
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

        printf("ok 2 - json\n");

    char *filename = malloc(256);
    sprintf(filename, "json/apirator-%02d_%02d_%d_%02d_%02d_%02d.json", day, mois, an, h, min, s);
    printf("ok 3 - json\n");
    fopen(filename, "a");
    printf("ok 4 - json\n");

    return filename;
}

int main(int argc, char* argv[]) {

    printf("ok 1 - main\n");

    if (argc != 3) {
        printf("Nombre d'arguments : %d \n", argc);
        printf("Erreur : nombre d'arguments incorrect.\n");
        return 1;
    }

    printf("ok 2 - main\n");

    Configuration config;

    strcpy(config.cle_api, argv[1]);
    strcpy(config.id_logement, argv[2]);
    strcpy(config.chemin_logs, creationLogs());
    strcpy(config.chemin_donnee, creationJson());
    printf("ok 3 - main\n");

    FILE *fichier = fopen("json/json_donnee.json", "w");

    printf("ok 4 - main\n");

    fprintf(fichier, "{\n");
    fprintf(fichier, "    \"cle_api\":\"%s\",\n", config.cle_api);
    fprintf(fichier, "    \"id_logement\":\"%s\",\n", config.id_logement);
    fprintf(fichier, "    \"chemin_logs\":\"%s\",\n", config.chemin_logs);
    fprintf(fichier, "    \"chemin_donnee\":\"%s\"\n", config.chemin_donnee);
    fprintf(fichier, "}\n");
    printf("ok 5 - Fin création du JSON dans le main\n");


    fclose(fichier);

    char command[64];
    sprintf(command, "./apirator %s %s", "json/json_donnee.json", "3D");
    system(command);
    printf("ok 6 - main\n");


    return 1;
}