#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

typedef struct {
    char *cle_api;
    char *id_logement;
    char *chemin_logs;
    char *chemin_donnee;
} Configuration;

void ecrireLogs(FILE *logs, char *message) {

    if (logs == NULL) {
        printf("Erreur : le fichier de logs n'est pas ouvert.\n");
        return;
    }

    if (message == NULL) {
        printf("Erreur : le message est NULL.\n");
        return;
    }

    int h, min, s, day, mois, an;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);   
        h = local->tm_hour;
        min = local->tm_min;
        s = local->tm_sec;
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

    // Ecrire le message dans le fichier de logs à la date courante
    fprintf(logs, "[%02d-%02d-%d]--[%02d:%02d:%02d]--> %s", day, mois, an, h, min, s, message);
    fprintf(logs, "\n");

    printf("[%02d-%02d-%d]--[%02d:%02d:%02d]--> %s\n", day, mois, an, h, min, s, message);
}

void utilisationJSON(const char *json_str, Configuration configFinal) {
    Configuration config;

    config.cle_api = malloc(256);
    config.id_logement = malloc(256);
    config.chemin_logs = malloc(256);
    config.chemin_donnee = malloc(256);
    

    if (sscanf(json_str, "    \"cle_api\":\"%[^\"]\",\n", config.cle_api) == 1) {
        strcpy(configFinal.cle_api, config.cle_api);
    } else if (sscanf(json_str, "    \"id_logement\":\"%[^\"]\",\n", config.id_logement) == 1) {
        strcpy(configFinal.id_logement, config.id_logement);
    } else if (sscanf(json_str, "    \"chemin_logs\":\"%[^\"]\",\n", config.chemin_logs) == 1) {
        strcpy(configFinal.chemin_logs, config.chemin_logs);
    } else if (sscanf(json_str, "    \"chemin_donnee\":\"%[^\"]\"\n", config.chemin_donnee) == 1) {
        strcpy(configFinal.chemin_donnee, config.chemin_donnee);
    }
}

int main(int argc, char *argv[]) {
    Configuration config;

    config.cle_api = malloc(256);
    config.id_logement = malloc(256);
    config.chemin_logs = malloc(256);
    config.chemin_donnee = malloc(256);


    if (argc != 2) {
        fprintf(stderr, "Utilisation : %s <fichier_json>\n", argv[0]);
        return 1;
    }

    // Ouvrir le fichier JSON
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s.\n", argv[1]);
        return 1;
    }

    char *json_data = NULL;
    size_t length = 0;
    ssize_t read;

    // Analyser le contenu JSON
    while ((read = getline(&json_data, &length, file)) != -1) {
        utilisationJSON(json_data, config);

        // Libération de la mémoire
        free(json_data);
        json_data = NULL;
        length = 0;
    }

        FILE *logs = fopen(config.chemin_logs, "a");


        if (config.cle_api == NULL || config.id_logement == NULL || config.chemin_logs == NULL || config.chemin_donnee == NULL) {
            ecrireLogs(logs, "Erreur lors de la récupération des données du fichier JSON");
            return 1;
        } else {
            ecrireLogs(logs, "Données récupérées avec succès");
        }
        
        ecrireLogs(logs, ("La clé API est : %s\n",config.cle_api));
        ecrireLogs(logs, ("L'id du logement est : %s\n",config.id_logement));
        ecrireLogs(logs, ("Le chemin vers les logs est : %s\n",config.chemin_logs));
        ecrireLogs(logs, ("Le chemin vers le fichier de données est : %s\n",config.chemin_donnee));

    fclose(file);

    return 0;
}