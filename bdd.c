#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int main() {
    const char *pghost = "127.0.0.1";
    const char *pgport = "5432";
    const char *dbName = "sae";
    const char *login = "sae";
    const char *pwd = "okai9xai9ufaFoht";

    char cle[20] = "123456789";
    char query[256];
    //int serveur;
    //serveur = open("serveur2bdd", O_RDONLY);
    
    //read(cle, serveur, 2);

    char conninfo[256];
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    //Ici je vais chercher l'id de la personne qui a la clé
    sprintf(query, "SELECT id_personnes FROM cle WHERE cle = '%s'", cle);
    PGresult *id_res = PQexec(conn, query);

    //Ici je vais chercher les privilège de la personne qui a la clé
    sprintf(query, "SELECT privilege FROM cle WHERE cle = '%s'", cle);
    PGresult *privilege = PQexec(conn, query);

    //Je verifie si il y a une personne avec cette clé
    if (PQntuples(id_res) > 0) {
        char *id_str = PQgetvalue(id_res, 0, 0);
        //Ici je vais chercher le nom de la personne qui a la clé
        sprintf(query, "SELECT nom_pers FROM personnes WHERE id = %s", id_str);
        PGresult *res = PQexec(conn, query);

        //Je verifie si il y'a bien quelqu'un avec cette id
        if (PQntuples(res) > 0) {
            printf("Nom de la personne a l'id %s : %s\n", id_str, PQgetvalue(res, 0, 0));
            printf("Privilege de la personne a l'id %s : %s\n", id_str, privilege);
            //Je verifie si la personne a des privilège
            if (PQgetvalue(privilege, 0, 0) == true) {
                printf("La personne a l'id %s a des privilège\n", id_str);
            } else {
                printf("La personne a l'id %s n'a pas de privilège\n", id_str);
            }
        //Si il n'y a pas de personne avec cette id alors on affiche un message d'erreur
        } else {
            printf("Aucune personne trouvée avec l'id %s\n", id_str);
        }
    //Si il n'y a pas de personne avec cette clé alors on affiche un message d'erreur
    } else {
        printf("Aucune clé trouvée correspondant à '%s'\n", cle);
    }

    PQclear(id_res);
    PQclear(privilege);
    PQfinish(conn);

    return 0;
}