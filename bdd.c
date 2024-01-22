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
    printf("Reçu : %s\n", cle);
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
                printf("-------------------------Début de la création du JSON------------------------\n");

                // Création d'un pointeur pour stocker les données
                size_t size = rows; // taille initiale estimée
                char *data = (char *)malloc(size * sizeof(char));

                write(cnx, "[\n", strlen("[\n"));
                printf("%s\n", data);
                for (int i = 0; i < rows; i++) {
                    write(cnx, "{\n", strlen("{\n"));
                    for (int j = 0; j < cols; j++) {
                        write(cnx, "    \"", strlen("    \""));
                        write(cnx, ("%s", PQfname(logement, j)), strlen(("    \"%s\": ", PQfname(logement, j))));
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
                printf("%s\n", data);
                
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

                    printf("-------------------------Début de la création du JSON------------------------\n");

                    // Création d'un pointeur pour stocker les données
                    size_t size = rows; // taille initiale estimée
                    char *data = (char *)malloc(size * sizeof(char));

                    write(cnx, "[\n", strlen("[\n"));
                    printf("%s\n", data);
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
                    printf("%s\n", data);
                    
                    PQclear(nom_logement);

                    printf("--------------------------Fin de la création du JSON-------------------------\n");
                    PQfinish(conn);
                    free(data);
                    return 1;
                } else {
                    printf("La personne n'a pas de logement\n");
                    PQclear(nom_logement);
                    PQfinish(conn);
                    return 0;
                }
            }
        } else {
            printf("La personne n'existe pas\n");
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
//**********************Code pour getLogement***********************//
//******************************************************************//
int getCalendrier(char cle[15], int cnx, char dateDebut, char dateFin) {
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

    printf("La clé est %s\n", cle);

    //Ici je vais chercher les privilège de la personne qui a la clé
    sprintf(query, "SELECT privilege FROM cle WHERE cle = '%s'", cle);
    PGresult *privilege = PQexec(conn, query);
    if (PQntuples(privilege) > 0) {
        sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
        PGresult *id_res = PQexec(conn, query);

        char *id_proprio = PQgetvalue(id_res, 0, 0);
        //Ici je vais chercher l'id d'un logement de la personne qui a la clé
        sprintf(query, "SELECT id_logement FROM logement WHERE id_proprio_logement = %s", id_proprio);
        PGresult *id_logement = PQexec(conn, query);
        printf("id_logement : %s\n", PQgetvalue(id_logement, 0, 0));

        //Ici je vais chercher le nom d'un logement de la personne qui a la clé
        sprintf(query, "SELECT libelle_logement FROM logement WHERE id_proprio_logement = %s", id_proprio);
        PGresult *nom_logement = PQexec(conn, query);
        printf("nom_logement : %s\n", PQgetvalue(nom_logement, 1, 0));

        if (PQntuples(id_logement) > 0) {

            write(cnx, "Voici la liste de vos logement : ", strlen("Voici la liste de vos logement : "));
            int rows = PQntuples(id_logement);
            printf("Rows : %d\n", rows);
            for (int i = 0; i < rows; i++) {
                write(cnx, ("(%s)", PQgetvalue(id_logement, i, 0)), strlen(("(%s)", PQgetvalue(id_logement, i, 0))));
                write(cnx, (" : %d\n", PQgetvalue(nom_logement, i, 0)), strlen((" : %d\n", PQgetvalue(nom_logement, i, 0))));
            }
            
            write(cnx, "Veuillez choisir un logement : ", strlen("Veuillez choisir un logement : "));
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
                int i = 0, n = 0;

                printf("-------------------------Début de la création du JSON------------------------\n");

                    // Création d'un pointeur pour stocker les données
                    size_t size = rows; // taille initiale estimée
                    char *data = (char *)malloc(size * sizeof(char));

                    write(cnx, "[\n", strlen("[\n"));
                    printf("%s\n", data);
                    while (i < rows && strcmp(PQgetvalue(date_Debut, i, n), dateFin)) {
                        write(cnx, "{\n", strlen("{\n"));
                        for (int j = 0; j < cols; j++) {
                            write(cnx, "    \"", strlen("    \""));
                            write(cnx, ("%s", PQfname(calendrier_Debut, j)), strlen(("%s", PQfname(calendrier_Debut, j))));
                            write(cnx, "\"", strlen("\""));
                            write(cnx, " : ", strlen(" : "));
                            write(cnx, ("%s", PQgetvalue(calendrier_Debut, i, j)), strlen(("%s", PQgetvalue(calendrier_Debut, i, j))));
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
                        n++;
                    }
                    write(cnx, "]\n", strlen("]\n"));
                    printf("%s\n", data);
                    
                    PQclear(nom_logement);

                    printf("--------------------------Fin de la création du JSON-------------------------\n");
                    PQfinish(conn);
                    free(data);
                    return 1;
            } else {
                printf("Il n'y a pas de réservation pour cette date\n");
                PQclear(calendrier_Debut);
                PQfinish(conn);
                return 0;
            }
        } else {
            printf("La personne n'a pas de logement\n");
            PQclear(nom_logement);
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