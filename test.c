#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include "bdd.h"

int main() {
    char input[15] = "123456789012345";
    int cnx = 0;
    char dateDebut[11] = "2019-01-01";
    char dateFin[11] = "2019-01-31";
    printf("verifCle: %d\n", verifCle(input));
    printf("getLogement: %d\n", getLogement(input, cnx));

    return 0;
}