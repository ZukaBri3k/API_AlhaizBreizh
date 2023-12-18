#include <postgresql/libpq-fe.h>
#include <stdio.h>

int main() {
    const char *conninfo = "host=localhost port=5432 dbname=sae user=postgres password=vae4ua9phuch4Cef";
    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur lors de la connexion à la base de données : %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PGresult *res = PQexec(conn, "SELECT id FROM personnes");
    printf("Nombre de lignes : %d\n", PQntuples(res));

    PQclear(res);
    PQfinish(conn);

    return 0;
}