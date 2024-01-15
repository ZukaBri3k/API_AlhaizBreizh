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
        return "Erreur lors de la connexion à la base de données";
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
                if (data == NULL) {
                    // gestion de l'erreur
                }
                //problème avec les strcat, ça concatène mais ça change l'emplacement du pointeur
                strcat(data, "[\n");
                printf("%s\n", data);
                for (int i = 0; i < rows; i++) {
                    strcat(data, "  {\n");
                    for (int j = 0; j < cols; j++) {
                        strcat(data, ("    \"%s\": \"%s\"", PQfname(logement, j), PQgetvalue(logement, i, j)));
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
                printf("%s\n", data);

                // si vous avez besoin de plus de mémoire :
                size *= 2; // double la taille
                char *new_data = (char *)realloc(data, size * sizeof(char));
                if (new_data == NULL) {
                    // gestion de l'erreur
                } else {
                    data = new_data;
                }

                // vous pouvez maintenant écrire plus de données dans 'data', jusqu'à 'size * sizeof(char)' octets
                // Convertir les données en format JSON et écrit dans data
                
                PQclear(logement);
                write(cnx, data, strlen(data));

                printf("--------------------------Fin de la création du JSON-------------------------\n");
                PQfinish(conn);
                free(data);
                return 1;
            } else
            {
                return "Vous n'avez pas les privilèges";
                PQfinish(conn);
            }
            
        }
    }
}