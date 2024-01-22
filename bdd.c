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
                printf("%s\n", data);
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

                    printf("\n-------------------------Début de la création du JSON------------------------\n");

                    // Création d'un pointeur pour stocker les données
                    size_t size = rows;
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
                    printf("%s\n", PQgetvalue(date_Debut, i, 0));
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
                }

                if (strcmp(PQgetvalue(date_Debut, i, 0), dateFin) == 0) {
                    printf("%s\n", PQgetvalue(date_Debut, i, 0));
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
        sprintf(query, "SELECT * FROM calendrier WHERE id_logement = %s ", input);
        PGresult *calendrier_Debut = PQexec(conn, query);

        int rows = PQntuples(calendrier_Debut);
        int cols = PQnfields(calendrier_Debut);
        int i = 0;

        //Ici je vais chercher les dates du début de la reservation de réservation du logement de la personne qui a la clé
        sprintf(query, "SELECT jour FROM calendrier WHERE id_logement = %s AND jour >= '%s' ", input, dateDebut);
        PGresult *date_Debut = PQexec(conn, query);

        while (i < rows && strcmp(PQgetvalue(date_Debut, i, 0), dateFin) != 0) {
            if (PQntuples(date_Debut) > 0) {
                char *query = ("INSERT INTO calendrier (statut_propriete, jour, disponibilite, tarif_journalier_location, duree_min_location, delai_res_arrivee, contrainte_arrivee, contrainte_depart, id_reserv, id_logement) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", 
                PQgetvalue(privilege, 0, 0), dateDebut, "false", PQgetvalue(calendrier_Debut, 0, 3), PQgetvalue(calendrier_Debut, 0, 4), 
                PQgetvalue(calendrier_Debut, 0, 5), PQgetvalue(calendrier_Debut, 0, 6), PQgetvalue(calendrier_Debut, 0, 7), PQgetvalue(calendrier_Debut, 0, 8), input);
                PGresult *res = PQexec(conn, query);

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                fprintf(stderr, "INSERT command failed: %s", PQerrorMessage(conn));
                PQclear(res);
                PQfinish(conn);
                return 1;
                }
            } else {
                char *query = "UPDATE calendrier SET disponibilite = 'false' WHERE id_logement = '%s' AND jour >= '%s'", input, dateDebut;
                PGresult *res = PQexec(conn, query);

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    fprintf(stderr, "UPDATE command failed: %s", PQerrorMessage(conn));
                    PQclear(res);
                    PQfinish(conn);
                    return 1;
                }
            }
            i++;
        }
        if (strcmp(PQgetvalue(date_Debut, i, 0), dateFin) == 0) {
            if (PQntuples(date_Debut) < 0) {
                char *query = ("INSERT INTO calendrier (statut_propriete, jour, disponibilite, tarif_journalier_location, duree_min_location, delai_res_arrivee, contrainte_arrivee, contrainte_depart, id_reserv, id_logement) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", 
                PQgetvalue(privilege, 0, 0), dateDebut, "false", PQgetvalue(calendrier_Debut, 0, 3), PQgetvalue(calendrier_Debut, 0, 4), 
                PQgetvalue(calendrier_Debut, 0, 5), PQgetvalue(calendrier_Debut, 0, 6), PQgetvalue(calendrier_Debut, 0, 7), PQgetvalue(calendrier_Debut, 0, 8), input);
                PGresult *res = PQexec(conn, query);

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                fprintf(stderr, "INSERT command failed: %s", PQerrorMessage(conn));
                PQclear(res);
                PQfinish(conn);
                return 1;
                }
            } else {
                char *query = "UPDATE calendrier SET disponibilite = 'false' WHERE id_logement = '%s' AND jour >= '%s'", input, dateDebut;
                PGresult *res = PQexec(conn, query);

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    fprintf(stderr, "UPDATE command failed: %s", PQerrorMessage(conn));
                    PQclear(res);
                    PQfinish(conn);
                    return 1;
                }
            }
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