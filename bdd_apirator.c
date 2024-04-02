#define _XOPEN_SOURCE
#include <time.h>
#include "bdd_apirator.h"
#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define LENCLE 20
#define BUFFSIZE 100
#define DATE 10


void ecrireLogs(FILE *logs, char *message) {

    int h, min, s, day, mois, an;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);   
        h = local->tm_hour;
        min = local->tm_min;
        s = local->tm_sec;
        day = local->tm_mday;
        mois = local->tm_mon + 1;
        an = local->tm_year + 1900;

    fprintf(logs, "[%02d-%02d-%d]--[%02d:%02d:%02d]--> %s", day, mois, an, h, min, s, message);
    fprintf(logs, "\n");

    printf("[%02d-%02d-%d]--[%02d:%02d:%02d]--> %s\n", day, mois, an, h, min, s, message);
}


bool verifCle(char cle[15], char *chemin_logs) {
    //******************************************************************//
    //************************Connexion a la bdd************************//
    //******************************************************************//
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";
    char conninfo[256];
    
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);

    FILE *logs = fopen(chemin_logs, "w");

    ecrireLogs(logs, "Connexion à la base de données");
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        ecrireLogs(logs, "Erreur lors de la connexion à la base de données");
        PQfinish(conn);
        return 1;
    }

    //******Création des variables******//
    char input[BUFFSIZE];
    char query[256];
    bool clebool = false;
    sleep(1);
    printf("\nReçu : %s", cle);
    cle[strcspn(cle, "\r\n\0")] = 0;

    //******************************************************************//
    //**********************Code pour verif la cle**********************//
    //******************************************************************//

    ecrireLogs(logs, "Vérification de la clé");
    ecrireLogs(logs, ("La clé reçu est : %s", cle));

        //Ici je vais chercher l'id de la personne qui a la clé
        sprintf(query, "SELECT id_personnes FROM cle WHERE cle = '%s'", cle);
        PGresult *id_res = PQexec(conn, query);
        if (PQntuples(id_res) > 0) {
            sleep(1);
            clebool = true;
            printf("La clé reçu est bonne\n");
            ecrireLogs(logs, "La clé reçu est bonne");
        } else {
            printf("La clé reçu est mauvaise\n");
            ecrireLogs(logs, "La clé reçu est mauvaise");
        }
        //Je verifie si il y a une personne avec cette clé
        //Si il n'y a pas de personne avec cette clé alors on envoie false
        PQclear(id_res);
        PQfinish(conn);
    return clebool;
}



//******************************************************************//
//**********************Code pour getDispo**********************//
//******************************************************************//
int getDispo(char cle[15], int idLogement, char dateDebut[12], char dateFin[12], char *chemin_logs, char *chemin_donnee) {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";
    char conninfo[256];
    
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    FILE *logs = fopen(chemin_logs, "w");
    FILE *json = fopen(chemin_donnee, "w");
//****Création des variables****//
    char query[256];

    sprintf(query, "SELECT disponibilite FROM cle WHERE cle = '%s'", cle);
    PGresult *cle_dispo = PQexec(conn, query);

    if (strcmp(PQgetvalue(cle_dispo, 0, 0), "t") == 0) {
        //Ici je vais chercher l'id d'un logement de la personne qui a la clé
        sprintf(query, "SELECT * FROM logement WHERE id_logement = %d", idLogement);
        PGresult *id_logement = PQexec(conn, query);
        
        ecrireLogs(logs, ("L'id du logement est : %d", idLogement));

        if (PQntuples(id_logement) > 0) {

            //Ici je vais chercher le calendrier de la reservation de réservation du logement de la personne qui a la clé
            sprintf(query, "SELECT * FROM calendrier WHERE id_logement = %d AND jour >= '%s'", idLogement, dateDebut);
            PGresult *calendrier_Debut = PQexec(conn, query);

            //Ici je vais chercher les dates du début de la reservation de réservation du logement de la personne qui a la clé
            sprintf(query, "SELECT jour FROM calendrier WHERE id_logement = %d AND jour >= '%s' AND jour <= '%s'", idLogement, dateDebut, dateFin);
            PGresult *date_Debut = PQexec(conn, query);

            int rows = PQntuples(date_Debut);
            int cols = PQnfields(calendrier_Debut);
            int i = 0;

            struct tm dateDebut_tm = {0};
            struct tm dateFin_tm = {0};

            dateDebut_tm.tm_isdst = -1;
            dateFin_tm.tm_isdst = -1;

            strptime(dateDebut, "%Y-%m-%d", &dateDebut_tm);
            strptime(dateFin, "%Y-%m-%d", &dateFin_tm);

            time_t start = mktime(&dateDebut_tm);
            time_t end = mktime(&dateFin_tm);

            int num_days = (end - start) / (24 * 60 * 60) + 1;

            printf("\n-------------------------Début de la création du JSON------------------------\n");

            ecrireLogs(logs, "Début de la création du JSON");

            fprintf(json, "[\n");

            char date_actuelle[12];
            strcpy(date_actuelle, dateDebut);

            struct tm date_actuelle_tm = dateDebut_tm;

            while (i < num_days && strcmp(date_actuelle, dateFin) != 0) {

                if(i < PQntuples(date_Debut)) {
                    //Ici je vais chercher les jour que je parcours actuellement
                    sprintf(query, "SELECT jour FROM calendrier WHERE id_logement = %d AND jour = '%s'", idLogement, PQgetvalue(date_Debut, i, 0));
                    PGresult *jour_check = PQexec(conn, query);
                    strcpy(date_actuelle, PQgetvalue(jour_check, 0, 0));

                    if (strcmp(PQgetvalue(calendrier_Debut, i, 2), "t") == 0) {

                        fprintf(json, "  {\n");
                        for (int j = 0; j < cols; j++) {
                            fprintf(json, ("    \"%s\" : ", PQfname(calendrier_Debut, j)));
                            //Ici je transforme les t en true et f en false car quand je souhiate récupérer les valeurs je récupère le boolean mais la fonction récupère que le 1er caractère du mot
                            if (strcmp(PQgetvalue(calendrier_Debut, i, j), "t") == 0) {
                                fprintf(json, "\"true\"");
                            } else {
                                fprintf(json, ("%s", PQgetvalue(calendrier_Debut, i, j)));
                            }

                            if (j < cols - 1) {
                                fprintf(json, ",");
                            }
                            fprintf(json, "\n");
                        } 
                        fprintf(json, "  }");
                        if (i < num_days - 1) {
                            fprintf(json, ",");
                        }
                        
                        fprintf(json, "\n");
                        fprintf(json, "");
                    }
                    PQclear(jour_check);
                } else {
                    strftime(date_actuelle, sizeof(date_actuelle), "%Y-%m-%d", &date_actuelle_tm);
                    date_actuelle_tm.tm_mday++;
                    mktime(&date_actuelle_tm);
                    //Ici c'est le cas ou la date n'est pas en base
                    fprintf(json, "  {\n");
                    fprintf(json, "    \"");
                    fprintf(json, ("%s", "jour"));
                    fprintf(json, "\"");
                    fprintf(json, " : ");
                    fprintf(json, "\"");
                    fprintf(json, ("%s", date_actuelle));
                    fprintf(json, "\"");
                    fprintf(json, ",");
                    fprintf(json, "\n");
                    
                    fprintf(json, "    \""); 
                    fprintf(json, ("%s", "disponibilite"));
                    fprintf(json, "\"");
                    fprintf(json, " : ");
                    fprintf(json, "true");
                    fprintf(json, ",");
                    fprintf(json, "\n");

                    fprintf(json, "  }");

                    if (i < num_days - 1) {
                        fprintf(json, ",");
                    }
                    fprintf(json, "\n");
                    fprintf(json, "");
                }
                i++;
            }
            fprintf(json, "]\n");
            fprintf(json, "");
            
            PQclear(id_logement);
            PQclear(calendrier_Debut);
            PQclear(date_Debut);
            PQclear(cle_dispo);
            ecrireLogs(logs, "Fin de la création du JSON");
            printf("\n--------------------------Fin de la création du JSON-------------------------\n");
            PQfinish(conn);
            return 1;
            
        } else {
            printf("\nLa personne n'a pas de logement\n");
            fprintf(json, "La personne n'a pas de logement\n");
            ecrireLogs(logs, "La personne n'a pas de logement");
            PQclear(id_logement);
            PQfinish(conn);
            return 0;
        }
    } else {
        printf("\nVous n'avez pas les droits pour effectuer cette action\n");
        fprintf(json, "Vous n'avez pas les droits pour effectuer cette action\n");
        ecrireLogs(logs, "Vous n'avez pas les droits pour effectuer cette action");
        PQclear(cle_dispo);
        PQfinish(conn);
        return 0;
    }
}