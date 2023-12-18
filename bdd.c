#include <postgresql/libpq-fe.h>
#include <stdio.h>

int main() {
    const char *pghost = "postgresdb";
    const char *pgport = "5432";
    const char *pgoptions = NULL;
    const char *pgtty = NULL;
    const char *name_bdd = "sae";
    const char *connexion = "sae";
    const char *pwd = "vae4ua9phuch4Cef";
    
    PGconn *conn(pghost, pgport, pgoptions, pgtty, name_bdd, connexion, pwd);
    
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