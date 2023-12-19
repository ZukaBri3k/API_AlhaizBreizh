#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

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


    sprintf(query, "SELECT id_personnes FROM cle WHERE cle = '%s'", cle);
    PGresult *id_res = PQexec(conn, query);

    if (PQntuples(id_res) > 0) {
        char *id_str = PQgetvalue(id_res, 0, 0);
        sprintf(query, "SELECT nom_pers FROM personnes WHERE id_personnes = %s", id_str);
        PGresult *res = PQexec(conn, query);
        if (PQntuples(res) > 0) {
            printf("Nom de la personne a l'id %s : %s\n", id_str, PQgetvalue(res, 0, 0));
        } else {
            printf("Aucune personne trouvée avec l'id %s\n", id_str);
        }
    } else {
        printf("Aucune clé trouvée correspondant à '%s'\n", cle);
    }

    PQclear(id_res);


    /* sprintf(query, "SELECT id_personnes FROM cle WHERE cle = '%s'", cle);
    PGresult *id = PQexec(conn, query);
    char *id_str = PQgetvalue(id, 0, 0);
    sprintf(query, "SELECT nom_pers FROM personnes WHERE id_personnes = '%s'", id_str);
    PGresult *res = PQexec(conn, query);
    printf("Nom de la personne a l'id 1 : %s\n", PQgetvalue(res, 0, 0)); */

    //PQclear(res);
    PQfinish(conn);

    return 0;
}