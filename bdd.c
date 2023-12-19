#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define LENCLE 20

int main() {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";

    char cle[LENCLE] = "";
    char input[50];
    char query[256];
    int serveur;
    int bdd;
    serveur = open("serveur2bdd", O_RDONLY);
    bdd = open("bdd2serveur", O_WRONLY);

    char conninfo[256];
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    
    while (1 == 1)
    {
        read(serveur, input, LENCLE);

        input[strcspn(input, "\r\n\0")] = 0;

        if (strstr(input, "cle") != NULL) {
            sscanf(input, "cle %s", cle);

            //Ici je vais chercher l'id de la personne qui a la clé
            sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
            PGresult *id_res = PQexec(conn, query);
            if (PQntuples(id_res) > 0) {
                write(bdd, "true", strlen("true"));
            } else {
                write(bdd, "false", strlen("false"));
            }
            //Je verifie si il y a une personne avec cette clé
            //Si il n'y a pas de personne avec cette clé alors on envoie false
            PQclear(id_res);
        } else {
            write(bdd, "false", strlen("false"));
        }

        if (strstr(input, "getLogement ") != NULL) {
            sscanf(input, "getLogement %s", cle);

            //Ici je vais chercher les privilège de la personne qui a la clé
            sprintf(query, "SELECT privilege FROM cle WHERE cle = '%s'", cle);
            PGresult *privilege = PQexec(conn, query);


            sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
            PGresult *id_res = PQexec(conn, query);


            char *id_str = PQgetvalue(id_res, 0, 0);
            //Ici je vais chercher le nom de la personne qui a la clé
            sprintf(query, "SELECT nom_pers FROM personnes WHERE id = %s", id_str);
            PGresult *res = PQexec(conn, query);


            //Je verifie si il y'a bien quelqu'un avec cette id
            if (PQntuples(res) > 0) {
                printf("Nom de la personne a l'id %s : %s\n", id_str, PQgetvalue(res, 0, 0));
                printf("Privilege de la personne a l'id %s : %s\n", id_str, PQgetvalue(privilege, 0, 0));

                //Je verifie si la personne a des privilèges
                if (strcmp(PQgetvalue(privilege, 0, 0), "t") == 0) {
                    printf("La personne a l'id %s a des privilèges\n", id_str);

                    //Ici je vais prendre tout les logements
                    sprintf(query, "SELECT libelle_logement FROM logement");
                    PGresult *logement = PQexec(conn, query);
                    for (int i = 0; i < PQntuples(logement); i++)
                    {
                        printf("Logement %s\n", PQgetvalue(logement, i, 0));
                    }
                    write(bdd, logement, strlen(logement));
                    PQclear(logement);
                } else {
                    printf("La personne a l'id %s n'a pas de privilèges\n", id_str);

                    //Ici je vais chercher le nom du logement de la personne qui a la clé
                    sprintf(query, "SELECT libelle_logement FROM logement WHERE id_proprio_logement = %s", id_str);
                    PGresult *nom_logement = PQexec(conn, query);

                    //Je verifie si la personne a un logement
                    if (PQntuples(nom_logement) > 0) {
                        for (int i = 0; i < PQntuples(nom_logement); i++)
                        {
                            printf("La personne a l'id %s est propriétaire du logement %s\n", id_str, PQgetvalue(nom_logement, i, 0));
                        }
                        write(bdd, nom_logement, strlen(nom_logement));
                    } else {
                        write(bdd, NULL, 2);
                    }
                    PQclear(nom_logement);
                }

            //Si il n'y a pas de personne avec cette id alors on affiche un message d'erreur
            } else {
                write(bdd, NULL, 2);
            }
            PQclear(res);
            PQclear(id_res);
            PQclear(privilege);
        }
    }
    PQfinish(conn);

    return 0;
}