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
    int serveur;
    serveur = open("serveur2bdd", O_RDONLY);
    
    read(cle, serveur, 2);

    char conninfo[256];
    sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            pghost, pgport, dbName, login, pwd);

    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PGresult *id = PQexec(conn, ("SELECT id_personnes FROM cle where cle = %s", cle));
    PGresult *res = PQexec(conn, ("SELECT nom FROM personnes where id_personnes = %s", id));
    printf("Nom de la personne a l'id 0 : %s\n", res);

    PQclear(res);
    PQfinish(conn);

    return 0;
}