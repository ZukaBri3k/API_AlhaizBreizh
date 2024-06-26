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

char *creationLogs(char *chemin_logs) {

    int day, mois, an;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

    char *filename = malloc(256);
    sprintf(filename, "%s/logs-%02d_%02d_%d.log", chemin_logs, day, mois, an);
    fopen(filename, "a");

    return filename;
}

char *creationJson(char *chemin_json) {

    int day, mois, an, h, min, s;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);
        h = local->tm_hour;
        min = local->tm_min;
        s = local->tm_sec;
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

    char *filename = malloc(256);
    sprintf(filename, "%s/apirator-%02d_%02d_%d_%02d_%02d_%02d.json", chemin_json, day, mois, an, h, min, s);
    fopen(filename, "a");

    return filename;
}

int main(int argc, char* argv[]) {

    if (argc != 6) {
        printf("Usage : <cle_api> <id_logement> <période> <chemin vers les logs> <chemin pour le retour des données>\n", argv[0]);
        return 1;
    }

    Configuration config;

    strcpy(config.cle_api, argv[1]);
    strcpy(config.id_logement, argv[2]);
    strcpy(config.chemin_logs, creationLogs(argv[4]));
    strcpy(config.chemin_donnee, creationJson(argv[5]));

    FILE *fichier = fopen("json/json_donnee.json", "w");

    fprintf(fichier, "{\n");
    fprintf(fichier, "    \"cle_api\":\"%s\",\n", config.cle_api);
    fprintf(fichier, "    \"id_logement\":\"%s\",\n", config.id_logement);
    fprintf(fichier, "    \"chemin_logs\":\"%s\",\n", config.chemin_logs);
    fprintf(fichier, "    \"chemin_donnee\":\"%s\"\n", config.chemin_donnee);
    fprintf(fichier, "}\n");

    fclose(fichier);

    char command[64];
    sprintf(command, "./apirator %s %s", "json/json_donnee.json", argv[3]);
    system(command);


    return 1;
}