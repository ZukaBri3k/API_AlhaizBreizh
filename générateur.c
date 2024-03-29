#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>

typedef struct {
    char *cle_api;
    char *id_logement;
    char *chemin_logs;
    char *chemin_donnee;
} Configuration;

FILE* creationLogs(FILE *logs) {

    int day, mois, an;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

    char filename[256];
    sprintf(filename, "logs-%02d_%02d_%d.json", day, mois, an);
    FILE *fichier_json = fopen(filename, "a");

    return fichier_json;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Erreur : nombre d'arguments incorrect.\n");
        return 1;
    }

    Configuration config;

    config.cle_api = argv[1];
    config.id_logement = argv[2];
    config.chemin_logs = argv[3];
    config.chemin_donnee = argv[4];

    FILE *fichier = fopen("json/json_donnee.json", "w");

    fprintf(fichier, "{\n");
    fprintf(fichier, "    \"cle_api\":\"%s\",\n", config.cle_api);
    fprintf(fichier, "    \"id_logement\":\"%s\",\n", config.id_logement);
    fprintf(fichier, "    \"chemin_logs\":\"%s\",\n", config.chemin_logs);
    fprintf(fichier, "    \"chemin_donnee\":\"%s\"\n", config.chemin_donnee);
    fprintf(fichier, "}\n");

    fclose(fichier);

    char command[64];
    sprintf(command, "./apirator %s", "json/json_donnee.json");
    system(command);

    return 1;
}