#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "bdd_apirator.h"


typedef struct {
    char *cle_api;
    char *id_logement;
    char *chemin_logs;
    char *chemin_donnee;
} Configuration;

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
    int N;
    char X;
    char *message = malloc(256);

    Configuration config;

    config.cle_api = malloc(256);
    config.id_logement = malloc(256);
    config.chemin_logs = malloc(256);
    config.chemin_donnee = malloc(256);


    if (argc != 3) {
        fprintf(stderr, "Utilisation : %s <fichier_json>, <période>\n", argv[0]);
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

    char logs[256];
    strcpy(logs, config.chemin_logs);


    if (config.cle_api == NULL || config.id_logement == NULL || config.chemin_logs == NULL || config.chemin_donnee == NULL) {
        ecrireLogs(logs, "Erreur lors de la récupération des données du fichier JSON");
        return 1;
    } else {
        ecrireLogs(logs, "Données récupérées avec succès");
    }
    
    ecrireLogs(config.chemin_logs, ("La clé API est : %s\n",config.cle_api));
    ecrireLogs(config.chemin_logs, ("L'id du logement est : %s\n",config.id_logement));
    ecrireLogs(config.chemin_logs, ("Le chemin vers les logs est : %s\n",config.chemin_logs));
    ecrireLogs(config.chemin_logs, ("Le chemin vers le fichier de données est : %s\n",config.chemin_donnee));


    if (sscanf(argv[2], "%d%c", &N, &X) != 2) {
        printf("Erreur : le format de la période est incorrect.\n");
        ecrireLogs(config.chemin_logs, "Erreur : le format de la période est incorrect.");
        return 1;
    }

    time_t T = time(NULL);
    struct tm temp_tm;
    struct tm *tm = localtime(&T);
    memcpy(&temp_tm, tm, sizeof(struct tm));
    switch (X) {
        case 'D':
            temp_tm.tm_mday += N;
            sprintf(message, "La période est de : %d jour", N);
            ecrireLogs(config.chemin_logs, message);
            break;
        case 'W':
            temp_tm.tm_mday += N * 7;
            sprintf(message, "La période est de : %d semaine", N);
            ecrireLogs(config.chemin_logs, message);
            break;
        case 'M':
            temp_tm.tm_mon += N;
            sprintf(message, "La période est de : %d mois", N);
            ecrireLogs(config.chemin_logs, message);
            break;
        default:
            printf("Erreur : le format de la période est incorrect.\n");
            ecrireLogs(config.chemin_logs, "Erreur : le format de la période est incorrect.");
            return 1;
    }

    mktime(&temp_tm);

    char buffer[256];
    char dateDebut[12];
    strftime(dateDebut, sizeof(dateDebut), "%Y-%m-%d", tm);
    printf("La date de début de la période est : %s\n", dateDebut);
    ecrireLogs(config.chemin_logs, ("La date de début de la période est : %s\n", dateDebut));
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &temp_tm);
    printf("La date de fin de la période est : %s\n", buffer);
    ecrireLogs(config.chemin_logs, ("La date de fin de la période est : %s\n", buffer));
    
    fclose(file);


    char cle[15];
    strcpy(cle, config.cle_api);

    int requete = 0;

    if(!verifCle(cle, config.chemin_logs)) {
        ecrireLogs(config.chemin_logs, "Clé API incorrecte");
        return 1;
    } else {
        ecrireLogs(config.chemin_logs, "Clé API correcte");
        int id_logement = atoi(config.id_logement);
        FILE *json = fopen(config.chemin_donnee, "w");

        requete = getDispo(cle, id_logement, dateDebut, buffer, config.chemin_logs, json);
    }

    if (requete == 1) {
        ecrireLogs(config.chemin_logs, "Requête effectuée avec succès");
    } else {
        ecrireLogs(config.chemin_logs, "Erreur lors de la requête");
    }
    

    return 0;
}