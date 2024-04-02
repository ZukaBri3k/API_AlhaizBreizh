#ifndef BDD_APIRATOR_H
#define BDD_APIRATOR_H

#include <stdbool.h>
#include <stdio.h>

extern void ecrireLogs(char *chemin, char *message);
extern bool verifCle(char input[15], char *chemin);
extern int getDispo(char input[15], int idLogement, char dateDebut[12], char dateFin[12], char *logs, FILE *json);

#endif // BDD_APIRATOR_H