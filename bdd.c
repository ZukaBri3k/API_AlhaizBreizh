#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define LENCLE 20
#define BUFFSIZE 100


// Fonction qui permet de créer le fichier json
void creerJson(PGresult *res) {
    //Ecriture du json
    FILE* json;
    char chemin[10] = "json.txt";
    // Création de la variables pour écrire dans le tube
    int bdd;

//************ Début de la création du json ************//
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    // Créer un tableau pour stocker les données
    char ***data = (char ***)malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        data[i] = (char **)malloc((cols + 1) * sizeof(char *));
    }

    // Remplir le tableau avec les données de la requête
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i][j] = strdup(PQgetvalue(res, i, j));
        }
    }

    // Convertir les données en format JSON et les écrits dans le tube
    // Ouverture du fichier JSON
    json = fopen(chemin, "w");

    /* printf("[\n"); */
    fprintf(json, "[\n");
    for (int i = 0; i < rows; i++) {
        /* printf("  {\n"); */
        fprintf(json, "  {\n");
        for (int j = 0; j < cols; j++) {
            /* printf("    \"%s\": \"%s\"", PQfname(res, j), data[i][j]); */
            fprintf(json, "    \"%s\": \"%s\"", PQfname(res, j), data[i][j]);
            if (j < cols - 1) {
                /* printf(","); */
                fprintf(json, ",");
            }
            /* printf("\n"); */
            fprintf(json, "\n");
        }
        /* printf("  }"); */
        fprintf(json, "  }");
        if (i < rows - 1) {
            /* printf(","); */
            fprintf(json, ",");
        }
        /* printf("\n"); */
        fprintf(json, "\n");
    }
    /* printf("]\n"); */
    fprintf(json, "]\n");
    char null_char = '\0';
    fwrite(&null_char, sizeof(char), 1, json);

    // Sleep pour laisser le temps au serveur de lire le fichier
    sleep(1);

    // Libérer la mémoire
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
    fclose(json);

    // Envoie du message au serveur pour dire que c'est fini
    bdd = open("bdd2serveur", O_WRONLY);
    write(bdd, "0", strlen("0"));
    close(bdd);
    PQclear(res);
}

int main() {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";

    char cle[LENCLE] = "";
    char query[256];
    // Création des variables pour les tubes
    int serveur;
    int bdd;

    int taille;
    printf("-------------------------------Début du programme-------------------------------\n");

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
        printf("\n-------------------------------Début de boucle-------------------------------\n");
        char input[BUFFSIZE];
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
                        
                        // Création du json
                        creerJson(logement);

                        printf("Fin de la création du json pour privilégier\n");
                    } else {

                        //Ici je vais chercher le nom du logement de la personne qui a la clé
                        sprintf(query, "SELECT * FROM logement WHERE id_proprio_logement = %s", id_str);
                        PGresult *nom_logement = PQexec(conn, query);

                        //Je verifie si la personne a un logement
                        if (PQntuples(nom_logement) > 0) {
                            
                            // Création du json
                            //creerJson(nom_logement);

                            printf("Fin de la création du json pour non privilégier\n");
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
                    bdd = write(bdd, "false", strlen("false"));
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
        printf("-------------------------------Fin de boucle-------------------------------\n");
    }
    PQfinish(conn);
    close(bdd);
    close(serveur);
    return 0;
}