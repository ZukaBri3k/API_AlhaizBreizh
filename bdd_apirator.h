#ifndef BDD_APIRATOR_H
#define BDD_APIRATOR_H

#include <stdbool.h>
#include <stdio.h>

extern bool verifCle(char input[15], char *chemin_logs);
extern int getDispo(char input[15], int idLogement, char dateDebut[12], char dateFin[12], char *chemin_logs, char *chemin_donnee);

#endif // BDD_APIRATOR_H