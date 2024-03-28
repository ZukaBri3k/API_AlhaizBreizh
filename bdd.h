#ifndef BDD_H
#define BDD_H

#include <stdbool.h>

extern bool verifCle(char input[15]);
extern int getLogement(char input[15], int cnx);
extern int getCalendrier(char input[15], int cnx, int idLogement, char dateDebut[12], char dateFin[12]);
extern int miseIndispo(char input[15], int cnx, int idLogement, char dateDebut[12], char dateFin[12]);
extern int miseDispo(char input[15], int cnx, int idLogement, char dateDebut[12], char dateFin[12]);
extern int getDispo(char input[15], int cnx, int idLogement, char dateDebut[12], char dateFin[12]);

#endif // BDD_H