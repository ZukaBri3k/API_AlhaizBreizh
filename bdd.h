#ifndef bdd.h
#define bdd.h
extern bool verifCle(char input[100]);
extern char* getLogement(char input[100]);
#endif // bdd.h

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

bool verifCle(char cle[100]) {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";
    char conninfo[256];
    int taille;
    char input[BUFFSIZE];
    char query[256];
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    printf("-------------------------------Début de boucle-------------------------------\n");
        bool clebool = false;
        input[taille] = '\0';
        sleep(1);
        printf("Reçu : %s\n", input);

        input[strcspn(input, "\r\n\0")] = 0;

        //******************************************************************//
        //******************************************************************//
        //******************************************************************//
        //**********************Code pour verif la cle**********************//
        //******************************************************************//
        //******************************************************************//
        //******************************************************************//

        if (strstr(input, "cle") != NULL) {
            sscanf(input, "cle %s", cle);

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
            else {
            printf("Commande incorrect\n");
            }
        printf("--------------------------------Fin de boucle--------------------------------\n");
        printf("-----------------------------------------------------------------------------\n");
    }
    return clebool;
}

char getLogement(char cle[100]) {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";
    char conninfo[256];
    int taille;
    char input[BUFFSIZE];
    char query[256];
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    if (strstr(input, "getLogement") != NULL) {
        sscanf(input, "getLogement %s", cle);
        printf("La clé est %s\n", cle);

        //Ici je vais chercher les privilège de la personne qui a la clé
        sprintf(query, "SELECT privilege FROM cle WHERE cle = '%s'", cle);
        PGresult *privilege = PQexec(conn, query);
        if (PQntuples(privilege) > 0)
        {
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

                    // Créer un tableau pour stocker les données
                    char *data = (char *)malloc(rows * sizeof(char *));

                    // Convertir les données en format JSON et écrit dans data
                    strcat(data, "[\n");
                    for (int i = 0; i < rows; i++) {
                        strcat(data, "  {\n");
                        for (int j = 0; j < cols; j++) {
                            strcat(data, ("    \"%s\": \"%s\"", PQfname(logement, j), data[i][j]));
                            if (j < cols - 1) {
                                strcat(data, ",");
                            }
                            strcat(data, "\n");
                        }
                        strcat(data, "  }");
                        if (i < rows - 1) {
                            strcat(data, ",");
                        }
                        strcat(data, "\n");
                    }
                    strcat(data, "]\n");

                    // Libérer la mémoire
                    char data_file[strlen(data) + 1];
                    free(data);
                    PQclear(logement);

                    printf("--------------------------Fin de la création du JSON-------------------------\n");
                    return data_file;
                }
            }
        }
    }
}
