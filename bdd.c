#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define LENCLE 20
#define BUFFSIZE 100

int main() {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";

    char cle[LENCLE] = "";
    char query[256];
    int serveur;
    int bdd;

    int taille;

    char input[BUFFSIZE];

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
        printf("-------------------------------Début de boucle-------------------------------\n");
        serveur = open("serveur2bdd", O_RDONLY);
        taille = read(serveur, input, BUFFSIZE - 1);
        input[taille] = '\0';
        sleep(1);
        close(serveur);
        printf("Reçu : %s\n", input);

        input[strcspn(input, "\r\n\0")] = 0;

        if (strstr(input, "cle") != NULL) {
            sscanf(input, "cle %s", cle);

            //Ici je vais chercher l'id de la personne qui a la clé
            sprintf(query, "SELECT id_proprio FROM cle WHERE cle = '%s'", cle);
            PGresult *id_res = PQexec(conn, query);
            if (PQntuples(id_res) > 0) {
                bdd = open("bdd2serveur", O_WRONLY);
                write(bdd, "true", strlen("true"));
                sleep(1);
                close(bdd);
                printf("La clé reçu est bonne\n");
            } else {
                bdd = open("bdd2serveur", O_WRONLY);
                write(bdd, "false", strlen("false"));
                sleep(1);
                close(bdd);
                printf("La clé reçu est mauvaise\n");
            }
            //Je verifie si il y a une personne avec cette clé
            //Si il n'y a pas de personne avec cette clé alors on envoie false
            PQclear(id_res);
        } else if (strstr(input, "getLogement") != NULL) {
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
                        char ***data = (char ***)malloc(rows * sizeof(char **));
                        for (int i = 0; i < rows; i++) {
                            data[i] = (char **)malloc((cols + 1) * sizeof(char *));
                        }

                        // Remplir le tableau avec les données de la requête
                        for (int i = 0; i < rows; i++) {
                            for (int j = 0; j < cols; j++) {
                                data[i][j] = strdup(PQgetvalue(logement, i, j));
                            }
                        }

                        // Convertir les données en format JSON et les écrits dans le tube ainsi que dans le fichier
                        FILE *json = fopen("json.txt", "w");
                        fprintf(json, "[\n");
                        for (int i = 0; i < rows; i++) {
                            fprintf(json, "  {\n");
                            for (int j = 0; j < cols; j++) {
                                fprintf(json, "    \"%s\": \"%s\"", PQfname(logement, j), data[i][j]);
                                if (j < cols - 1) {
                                    fprintf(json, ",");
                                }
                                fprintf(json, "\n");
                            }
                            fprintf(json, "  }");
                            if (i < rows - 1) {
                                fprintf(json, ",");
                            }
                            fprintf(json, "\n");
                        }
                        fprintf(json, "]\n");
                        char null_char = '\0';
                        fwrite(&null_char, sizeof(char), 1, json);
                        fclose(json);

                        // Ecrit le JSON dans le tube
                        bdd = open("bdd2serveur", O_WRONLY);
                        write(bdd, "0", strlen("0"));
                        sleep(1);

                        // Libérer la mémoire
                        for (int i = 0; i < rows; i++) {
                            for (int j = 0; j < cols; j++) {
                                free(data[i][j]);
                            }
                            free(data[i]);
                        }
                        free(data);
                        close(bdd);

                        PQclear(logement);

                        printf("--------------------------Fin de la création du JSON-------------------------\n");
                    } else {

                        //Ici je vais chercher le nom du logement de la personne qui a la clé
                        sprintf(query, "SELECT * FROM logement WHERE id_proprio_logement = %s", id_str);
                        PGresult *nom_logement = PQexec(conn, query);

                        //Je verifie si la personne a un logement
                        if (PQntuples(nom_logement) > 0) {
                            
                            int rows = PQntuples(nom_logement);
                            int cols = PQnfields(nom_logement);

                            printf("-------------------------Début de la création du JSON------------------------\n");

                            // Créer un tableau pour stocker les données
                            char ***data = (char ***)malloc(rows * sizeof(char **));
                            for (int i = 0; i < rows; i++) {
                                data[i] = (char **)malloc((cols + 1) * sizeof(char *));
                            }

                            // Remplir le tableau avec les données de la requête
                            for (int i = 0; i < rows; i++) {
                                for (int j = 0; j < cols; j++) {
                                    data[i][j] = strdup(PQgetvalue(nom_logement, i, j));
                                }
                            }

                            // Convertir les données en format JSON et les écrits dans le tube ainsi que dans le fichier
                            FILE *json = fopen("json.txt", "w");
                            fprintf(json, "[\n");
                            for (int i = 0; i < rows; i++) {
                                fprintf(json, "  {\n");
                                for (int j = 0; j < cols; j++) {
                                    fprintf(json, "    \"%s\": \"%s\"", PQfname(nom_logement, j), data[i][j]);
                                    if (j < cols - 1) {
                                        fprintf(json, ",");
                                    }
                                    fprintf(json, "\n");
                                }
                                fprintf(json, "  }");
                                if (i < rows - 1) {
                                    fprintf(json, ",");
                                }
                                fprintf(json, "\n");
                            }
                            fprintf(json, "]\n");
                            char null_char = '\0';
                            fwrite(&null_char, sizeof(char), 1, json);
                            fclose(json);

                            // Ecrit le JSON dans le tube
                            bdd = open("bdd2serveur", O_WRONLY);
                            write(bdd, "0", strlen("0"));
                            sleep(1);

                            // Libérer la mémoire
                            for (int i = 0; i < rows; i++) {
                                for (int j = 0; j < cols; j++) {
                                    free(data[i][j]);
                                }
                                free(data[i]);
                            }
                            free(data);
                            close(bdd);

                            printf("--------------------------Fin de la création du JSON-------------------------\n");
                        } else {
                            open("bdd2serveur", O_WRONLY);
                            write(bdd, "false", strlen("false"));
                            sleep(1);
                            close(bdd);
                        }
                        PQclear(nom_logement);
                    }
                //Si il n'y a pas de personne avec cette id alors on affiche un message d'erreur et on renvoie null
                } else {
                    bdd = open("bdd2serveur", O_WRONLY);
                    write(bdd, "false", strlen("false"));
                    sleep(1);
                    close(bdd);
                    printf("Il n'y a pas de personne avec l'id %s\n", id_str);
                }
                PQclear(res);
                PQclear(id_res);
            } else {
                bdd = open("bdd2serveur", O_WRONLY);
                write(bdd, "false", strlen("false"));
                sleep(1);
                close(bdd);
                printf("La clé reçu est mauvaise\n");
            }
            PQclear(privilege);
        } else {
            bdd = open("bdd2serveur", O_WRONLY);
            write(bdd, "Commande incorrect", strlen("Commande incorrect"));
            close(bdd);
            printf("Commande incorrect\n");
        }
        printf("--------------------------------Fin de boucle--------------------------------\n");
        printf("-----------------------------------------------------------------------------\n");
    }
    PQfinish(conn);
    return 0;
}