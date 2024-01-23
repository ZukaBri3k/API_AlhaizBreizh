#define _XOPEN_SOURCE
#include <time.h>
#include "bdd.h"
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

bool verifCle(char cle[15]) {
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
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
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

        //Ici je vais chercher l'id de la personne qui a la clé
        sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
        PGresult *id_res = PQexec(conn, query);
        if (PQntuples(id_res) > 0) {
            sleep(1);
            clebool = true;
            printf("La clé reçu est bonne\n");
        } else {
            printf("La clé reçu est mauvaise\n");
        }
        //Je verifie si il y a une personne avec cette clé
        //Si il n'y a pas de personne avec cette clé alors on envoie false
        PQclear(id_res);
        PQfinish(conn);
    return clebool;
}




//******************************************************************//
//**********************Code pour getLogement***********************//
//******************************************************************//
int getLogement(char cle[15], int cnx) {
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
//****Création des variables****//
    char input[BUFFSIZE];
    char query[256];

    printf("La clé est %s", cle);

    //Ici je vais chercher les privilège de la personne qui a la clé
    sprintf(query, "SELECT privilege FROM cle WHERE cle = '%s'", cle);
    PGresult *privilege = PQexec(conn, query);
    if (PQntuples(privilege) > 0) {
        sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
        PGresult *id_res = PQexec(conn, query);

        char *id_str = PQgetvalue(id_res, 0, 0);
        //Ici je vais chercher le nom de la personne qui a la clé
        sprintf(query, "SELECT nom_pers FROM personnes WHERE id = %s", id_str);
        PGresult *res = PQexec(conn, query);

        //Je verifie si il y'a bien quelqu'un avec cette id
        if (PQntuples(res) > 0) {

            //Je verifie si la personne a des privilèges
            if (strcmp(PQgetvalue(privilege, 0, 0), "t") == 0) {

                //Ici je vais prendre tout les logements
                sprintf(query, "SELECT * FROM logement");
                PGresult *logement = PQexec(conn, query);
                
                int rows = PQntuples(logement);
                int cols = PQnfields(logement);
                printf("\n-------------------------Début de la création du JSON------------------------\n");

                // Création d'un pointeur pour stocker les données
                size_t size = rows; // taille initiale estimée
                char *data = (char *)malloc(size * sizeof(char));

                write(cnx, "[\n", strlen("[\n"));
                for (int i = 0; i < rows; i++) {
                    write(cnx, "{\n", strlen("{\n"));
                    for (int j = 0; j < cols; j++) {
                        write(cnx, "    \"", strlen("    \""));
                        write(cnx, ("%s", PQfname(logement, j)), strlen(("%s", PQfname(logement, j))));
                        write(cnx, "\"", strlen("\""));
                        write(cnx, " : ", strlen(" : "));
                        write(cnx, ("%s", PQgetvalue(logement, i, j)), strlen(("%s", PQgetvalue(logement, i, j))));
                        if (j < cols - 1) {
                            write(cnx, ",", strlen(","));
                        }
                        write(cnx, "\n", strlen("\n"));
                    }
                    write(cnx, "  }", strlen("  }"));
                    if (i < rows - 1) {
                        write(cnx, ",", strlen(","));
                    }
                    write(cnx, "\n", strlen("\n"));
                }
                write(cnx, "]\n", strlen("]\n"));
                write(cnx, "", strlen(""));
                
                PQclear(logement);

                printf("--------------------------Fin de la création du JSON-------------------------\n");
                PQfinish(conn);
                free(data);
                return 1;
            } else {

                //Ici je vais chercher le nom du logement de la personne qui a la clé
                sprintf(query, "SELECT * FROM logement WHERE id_proprio_logement = %s", id_str);
                PGresult *nom_logement = PQexec(conn, query);

                //Je verifie si la personne a un logement
                if (PQntuples(nom_logement) > 0) {
                    
                    int rows = PQntuples(nom_logement);
                    int cols = PQnfields(nom_logement);

                    printf("\n-------------------------Début de la création du JSON------------------------\n");

                    // Création d'un pointeur pour stocker les données
                    size_t size = rows;
                    char *data = (char *)malloc(size * sizeof(char));

                    write(cnx, "[\n", strlen("[\n"));
                    for (int i = 0; i < rows; i++) {
                        write(cnx, "{\n", strlen("{\n"));
                        for (int j = 0; j < cols; j++) {
                            write(cnx, "    \"", strlen("    \""));
                            write(cnx, ("%s", PQfname(nom_logement, j)), strlen(("%s", PQfname(nom_logement, j))));
                            write(cnx, "\"", strlen("\""));
                            write(cnx, " : ", strlen(" : "));
                            write(cnx, ("%s", PQgetvalue(nom_logement, i, j)), strlen(("%s", PQgetvalue(nom_logement, i, j))));
                            if (j < cols - 1) {
                                write(cnx, ",", strlen(","));
                            }
                            write(cnx, "\n", strlen("\n"));
                        }
                        write(cnx, "  }", strlen("  }"));
                        if (i < rows - 1) {
                            write(cnx, ",", strlen(","));
                        }
                        write(cnx, "\n", strlen("\n"));
                    }
                    write(cnx, "]\n", strlen("]\n"));
                    write(cnx, "", strlen(""));
                    
                    PQclear(nom_logement);
                    PQclear(res);
                    PQclear(id_res);

                    printf("--------------------------Fin de la création du JSON-------------------------\n");
                    PQfinish(conn);
                    free(data);
                    return 1;
                } else {
                    printf("La personne n'a pas de logement\n");
                    write(cnx, "La personne n'a pas de logement\n", strlen("La personne n'a pas de logement\n"));
                    PQclear(nom_logement);
                    PQclear(id_res);
                    PQclear(res);
                    PQfinish(conn);
                    return 0;
                }
            }
        } else {
            printf("La personne n'existe pas\n");
            write(cnx, "La personne n'existe pas\n", strlen("La personne n'existe pas\n"));
            PQclear(res);
            PQfinish(conn);
            return 0;
        }
    } else { 
        printf("La clé n'existe pas\n");
        PQclear(privilege);
        PQfinish(conn);
        return 0;
    }
}




//******************************************************************//
//**********************Code pour getCalendier**********************//
//******************************************************************//
int getCalendrier(char cle[15], int cnx, char dateDebut[12], char dateFin[12]) {
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
//****Création des variables****//
    char input[BUFFSIZE];
    char query[256];

    sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
    PGresult *id_res = PQexec(conn, query);

    char *id_proprio = PQgetvalue(id_res, 0, 0);
    //Ici je vais chercher l'id d'un logement de la personne qui a la clé
    sprintf(query, "SELECT id_logement FROM logement WHERE id_proprio_logement = %s", id_proprio);
    PGresult *id_logement = PQexec(conn, query);

    //Ici je vais chercher le nom d'un logement de la personne qui a la clé
    sprintf(query, "SELECT libelle_logement FROM logement WHERE id_proprio_logement = %s", id_proprio);
    PGresult *nom_logement = PQexec(conn, query);

    if (PQntuples(id_logement) > 0) {

        write(cnx, "Voici la liste de vos logement : \n", strlen("Voici la liste de vos logement : \n"));
        int ligne = PQntuples(id_logement);
        for (int i = 0; i < ligne ; i++) {
            write(cnx, "(", strlen("("));
            write(cnx, ("%s", PQgetvalue(id_logement, i, 0)), strlen(("%s", PQgetvalue(id_logement, i, 0))));
            write(cnx, ")", strlen(")"));
            write(cnx, " : ", strlen(" : "));
            write(cnx, ("%d", PQgetvalue(nom_logement, i, 0)), strlen(("%d", PQgetvalue(nom_logement, i, 0))));
            write(cnx, "\n", strlen("\n"));
        }
        
        write(cnx, "Veuillez choisir l'id de l'un logement : ", strlen("Veuillez choisir l'id de l'un logement : "));
        read(cnx, input, BUFFSIZE);
        input[strcspn(input, "\r\n\0")] = 0;
        printf("Reçu : %s\n", input);

        //Ici je vais chercher le calendrier de la reservation de réservation du logement de la personne qui a la clé
        sprintf(query, "SELECT * FROM calendrier WHERE id_logement = %s AND jour >= '%s' ", input, dateDebut);
        PGresult *calendrier_Debut = PQexec(conn, query);
        if (PQntuples(calendrier_Debut) > 0) {

            //Ici je vais chercher les dates du début de la reservation de réservation du logement de la personne qui a la clé
            sprintf(query, "SELECT jour FROM calendrier WHERE id_logement = %s AND jour >= '%s' ", input, dateDebut);
            PGresult *date_Debut = PQexec(conn, query);

            int rows = PQntuples(calendrier_Debut);
            int cols = PQnfields(calendrier_Debut);
            int i = 0;

            printf("\n-------------------------Début de la création du JSON------------------------\n");

                // Création d'un pointeur pour stocker les données
                size_t size = rows;
                char *data = (char *)malloc(size * sizeof(char));

                write(cnx, "[\n", strlen("[\n"));
                while (i < rows && strcmp(PQgetvalue(date_Debut, i, 0), dateFin) != 0) {
                    write(cnx, "  {\n", strlen("  {\n"));
                    for (int j = 0; j < cols; j++) {
                        write(cnx, "    \"", strlen("    \"")); 
                        write(cnx, ("%s", PQfname(calendrier_Debut, j)), strlen(("%s", PQfname(calendrier_Debut, j))));
                        write(cnx, "\"", strlen("\""));
                        write(cnx, " : ", strlen(" : "));
                        //Ici je transforme les t en true et f en false car quand je souhiate récupérer les valeurs je récupère le boolean mais la fonction récupère que le 1er caractère du mot
                        if (strcmp(PQgetvalue(calendrier_Debut, i, j), "t") == 0) {
                            write(cnx, "true", strlen("true"));
                        } else if (strcmp(PQgetvalue(calendrier_Debut, i, j), "f") == 0) {
                            write(cnx, "false", strlen("false"));
                        } else {
                            write(cnx, ("%s", PQgetvalue(calendrier_Debut, i, j)), strlen(("%s", PQgetvalue(calendrier_Debut, i, j))));
                        }

                        if (j < cols - 1) {
                            write(cnx, ",", strlen(","));
                        }
                        write(cnx, "\n", strlen("\n"));
                    } 
                    write(cnx, "  }", strlen("  }"));
                    if (i < rows - 1) {
                        write(cnx, ",", strlen(","));
                    }
                    write(cnx, "\n", strlen("\n"));
                    i++;
                    write(cnx, "", strlen(""));
                }

                if (strcmp(PQgetvalue(date_Debut, i, 0), dateFin) == 0) {
                    write(cnx, "  {\n", strlen("  {\n"));
                    for (int j = 0; j < cols; j++) {
                        write(cnx, "    \"", strlen("    \"")); 
                        write(cnx, ("%s", PQfname(calendrier_Debut, j)), strlen(("%s", PQfname(calendrier_Debut, j))));
                        write(cnx, "\"", strlen("\""));
                        write(cnx, " : ", strlen(" : "));
                        //Ici je transforme les t en true et f en false car quand je souhiate récupérer les valeurs je récupère le boolean mais la fonction récupère que le 1er caractère du mot
                        if (strcmp(PQgetvalue(calendrier_Debut, i, j), "t") == 0) {
                            write(cnx, "true", strlen("true"));
                        } else if (strcmp(PQgetvalue(calendrier_Debut, i, j), "f") == 0) {
                            write(cnx, "false", strlen("false"));
                        } else {
                            write(cnx, ("%s", PQgetvalue(calendrier_Debut, i, j)), strlen(("%s", PQgetvalue(calendrier_Debut, i, j))));
                        }

                        if (j < cols - 1) {
                            write(cnx, ",", strlen(","));
                        }
                        write(cnx, "\n", strlen("\n"));
                    } 
                    write(cnx, "  }", strlen("  }"));
                    if (i < rows - 1) {
                        write(cnx, ",", strlen(","));
                    }
                    write(cnx, "\n", strlen("\n"));
                    write(cnx, "", strlen(""));
                }
                
                write(cnx, "]\n", strlen("]\n"));
                printf("%s\n", data);
                
                PQclear(nom_logement);
                PQclear(id_logement);
                PQclear(id_res);
                PQclear(calendrier_Debut);

                printf("\n--------------------------Fin de la création du JSON-------------------------\n");
                PQfinish(conn);
                free(data);
                return 1;
        } else {
            printf("Il n'y a pas de réservation pour ces date\n");
            write(cnx, "Il n'y a pas de réservation pour ces date\n", strlen("Il n'y a pas de réservation pour ces date\n"));
            PQclear(calendrier_Debut);
            PQclear(nom_logement);
            PQclear(id_logement);
            PQclear(id_res);
            PQfinish(conn);
            return 0;
        }
    } else {
        printf("La personne n'a pas de logement\n");
        write(cnx, "La personne n'a pas de logement\n", strlen("La personne n'a pas de logement\n"));
        PQclear(nom_logement);
        PQclear(id_logement);
        PQclear(id_res);
        PQfinish(conn);
        return 0;
    }
}




//******************************************************************//
//**********************Code pour miseIndispo***********************//
//******************************************************************//
int miseIndispo(char cle[15], int cnx, char dateDebut[12], char dateFin[12]) {
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
//****Création des variables****//
    char input[BUFFSIZE];
    char query[256];

    sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
    PGresult *id_res = PQexec(conn, query);

    char *id_proprio = PQgetvalue(id_res, 0, 0);
    //Ici je vais chercher l'id d'un logement de la personne qui a la clé
    sprintf(query, "SELECT id_logement FROM logement WHERE id_proprio_logement = %s", id_proprio);
    PGresult *id_logement = PQexec(conn, query);

    //Ici je vais chercher le nom d'un logement de la personne qui a la clé
    sprintf(query, "SELECT libelle_logement FROM logement WHERE id_proprio_logement = %s", id_proprio);
    PGresult *nom_logement = PQexec(conn, query);

    if (PQntuples(id_logement) > 0) {

        write(cnx, "Voici la liste de vos logement : \n", strlen("Voici la liste de vos logement : \n"));
        int ligne = PQntuples(id_logement);
        for (int i = 0; i < ligne ; i++) {
            write(cnx, "(", strlen("("));
            write(cnx, ("%s", PQgetvalue(id_logement, i, 0)), strlen(("%s", PQgetvalue(id_logement, i, 0))));
            write(cnx, ")", strlen(")"));
            write(cnx, " : ", strlen(" : "));
            write(cnx, ("%d", PQgetvalue(nom_logement, i, 0)), strlen(("%d", PQgetvalue(nom_logement, i, 0))));
            write(cnx, "\n", strlen("\n"));
        }
        
        write(cnx, "Veuillez choisir l'id du logement à modifier : ", strlen("Veuillez choisir l'id du logement à modifier : "));
        read(cnx, input, BUFFSIZE);
        input[strcspn(input, "\r\n\0")] = 0;
        printf("Reçu : %s\n", input);
        
        //Ici je vais chercher les privilège de la personne qui a la clé
        sprintf(query, "SELECT privilege FROM cle WHERE cle = '%s'", cle);
        PGresult *privilege = PQexec(conn, query);

        //Ici je vais chercher le calendrier de la reservation de réservation du logement de la personne qui a la clé
        sprintf(query, "SELECT * FROM calendrier WHERE id_logement = %s", input);
        PGresult *calendrier_Debut = PQexec(conn, query);

        //Ici je vais chercher les dates du début de la reservation de réservation du logement de la personne qui a la clé
        sprintf(query, "SELECT jour FROM calendrier WHERE id_logement = %s AND jour >= '%s'", input, dateDebut);
        PGresult *date_Debut = PQexec(conn, query);
        int rows = PQntuples(date_Debut);
        int i = 0;
        while (i < rows && strcmp(PQgetvalue(date_Debut, i, 0), dateFin) != 0) {
            if (PQntuples(date_Debut) <= 0 && strcmp(dateDebut, dateFin) > 0) {
                printf("La ligne n'existe pas\n");
                write(cnx, "La ligne n'existe pas\n", strlen("La ligne n'existe pas\n"));
            } else {
                char query[1024];
                sprintf(query, "UPDATE calendrier SET disponibilite = 'false' WHERE id_logement = '%s' AND jour >= '%s'", input, dateDebut);
                PGresult *res = PQexec(conn, query);
                printf("Changement de disponibilité fait\n");
                write(cnx, "Mise en indisponibilité pour le logement : \n", strlen("Mise en indisponibilité pour le logement :\n"));
                write(cnx, input, strlen(input));
                write(cnx, " réussi pour les dates : ", strlen(" réussi pour les dates : "));
                write(cnx, dateDebut, strlen(dateDebut));
                write(cnx, " - ", strlen(" - "));
                write(cnx, dateFin, strlen(dateFin));
                write(cnx, "\n", strlen("\n"));

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    fprintf(stderr, "UPDATE command failed: %s", PQerrorMessage(conn));
                    PQclear(res);
                    PQfinish(conn);
                    return 1;
                }
            }
            i++;
        }
        printf("Ligne : %d\n", PQntuples(date_Debut));
        printf("Date : %s\n", dateFin);
        if (PQntuples(date_Debut) <= 0 && strcmp(dateDebut, dateFin) < 0) {

                struct tm dateDebut_tm = {0};
                struct tm dateFin_tm = {0};

                dateDebut_tm.tm_isdst = -1; // Permet à mktime de déterminer si l'heure d'été est en vigueur
                dateFin_tm.tm_isdst = -1; // Permet à mktime de déterminer si l'heure d'été est en vigueur

                strptime(dateDebut, "%Y-%m-%d", &dateDebut_tm);
                strptime(dateFin, "%Y-%m-%d", &dateFin_tm);

                time_t start = mktime(&dateDebut_tm);
                time_t end = mktime(&dateFin_tm);

                printf("start : %ld\n", start);
                printf("end : %ld\n", end);

                // Calculer le nombre de jours entre les deux dates
                int num_days = (end - start) / (24 * 60 * 60);
                printf("Nombre de jours : %d\n", num_days);

                for (int j = 0; j <= num_days; j++) {
                    // Ajouter j jours à la date de début
                    time_t current = start + j * 24 * 60 * 60;

                    struct tm *current_tm = localtime(&current);

                    char current_date[11];
                    strftime(current_date, sizeof(current_date), "%Y-%m-%d", current_tm);

                    char booleen[6] = "false";

                    if(strcmp(PQgetvalue(calendrier_Debut, i, 0), "t") == 0) {
                        strcpy(booleen, "true");
                    }

                    printf("Date : %s\n", current_date);

                    char query[1024];
                    sprintf(query, "INSERT INTO calendrier (statut_propriete, jour, disponibilite, tarif_journalier_location, duree_min_location, delai_res_arrivee, contrainte_arrivee, contrainte_depart, id_reserv, id_logement) VALUES ('%s', '%s', 'false', 0, 0, 0, NULL, NULL, NULL, '%s')", 
                    booleen, current_date, input);
                    PGresult *res = PQexec(conn, query);

                    printf("Création réussi\n");
                    write(cnx, "Création réussi\n", strlen("Création réussi\n"));

                    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                        fprintf(stderr, "INSERT command failed: %s", PQerrorMessage(conn));
                        PQclear(res);
                        PQfinish(conn);
                        return 1;
                    }

                    PQclear(res);
                }
            } else {
                printf("La ligne existe déjà\n");
                write(cnx, "La ligne existe déjà\n", strlen("La ligne existe déjà\n"));
            }

        PQclear(calendrier_Debut);
        PQclear(nom_logement);
        PQclear(id_logement);
        PQclear(id_res);
        PQclear(date_Debut);
        PQclear(privilege);
        PQfinish(conn);
        return 1;

    } else {
        printf("La personne n'a pas de logement\n");
        write(cnx, "La personne n'a pas de logement\n", strlen("La personne n'a pas de logement\n"));
        PQclear(nom_logement);
        PQclear(id_logement);
        PQclear(id_res);
        PQfinish(conn);
        return 0;
    }
}