#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include "bdd.h"

#define BUFFER_SIZE 1024

void afficherHeure(FILE *logs) {

    int h, min, s, day, mois, an;
    time_t now = time(NULL);

    struct tm *local = localtime(&now);
        h = local->tm_hour;        
        min = local->tm_min;       
        s = local->tm_sec;       
        day = local->tm_mday;          
        mois = local->tm_mon + 1;     
        an = local->tm_year + 1900;  
    // Afficher la date courante
    fprintf(logs, "[%02d-%02d-%d]--", day, mois, an);
    fprintf(logs, "[%02d:%02d:%02d] > ", h, min, s);

    printf("[%02d-%02d-%d]--", day, mois, an);
    printf("[%02d:%02d:%02d] > ", h, min, s);
}

void afficherHeureIP(FILE *logs, struct sockaddr_in conn_addr) {

    int h, min, s, day, mois, an;
    time_t now = time(NULL);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(conn_addr.sin_addr), ip, INET_ADDRSTRLEN);

    struct tm *local = localtime(&now);
        h = local->tm_hour;        
        min = local->tm_min;       
        s = local->tm_sec;       
        day = local->tm_mday;          
        mois = local->tm_mon + 1;     
        an = local->tm_year + 1900;  
    // Afficher la date courante
    fprintf(logs, "[%02d-%02d-%d]--", day, mois, an);
    fprintf(logs, "[%02d:%02d:%02d]--[%s] > ", h, min, s, ip);

    printf("[%02d-%02d-%d]--", day, mois, an);
    printf("[%02d:%02d:%02d]--[%s] > ", h, min, s, ip);
}

int main(int argc, char* argv[]) {

    int PORT = 8080;
    bool VERBOSE = false;
    FILE *logs = fopen("logs.txt", "a");
    char cleAPI[20];
    
    //traitement des options longues
    int opt;
    int option_index = 0;

    //liste des options longues
    struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {"verbose", no_argument, NULL, 'v'}
    };


    //parcours de toutes les options
    while((opt = getopt_long(argc, argv, ":p:n", long_options, &option_index)) != -1) 
    { 
        switch(opt)
        { 
            case 'p': 
                //si une option de port est donnée
                afficherHeure(logs);
                printf("Port personnalisé : %s\n", optarg);
                PORT = atoi(optarg);
                break;
            case 'h':
                //renvoie l'aide
                afficherHeure(logs);
                printf("help\n");
                break;
            case 'v':
                //active le mode verbose
                VERBOSE = true;
                afficherHeure(logs);
                printf("Verbose activé\n");
                break;
        }
    }

    //Variable socket
    int sock;
    int ret;
    struct sockaddr_in addr;
    int size;
    int cnx;
    struct sockaddr_in conn_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    if (VERBOSE)
    {
        afficherHeure(logs);
        fprintf(logs, "bind=%d\n", ret);
        printf("bind=%d\n", ret);
    } 

    //affichage du démarrage
    afficherHeure(logs);
    fprintf(logs, "Serveur démarré sur le port %d\n", PORT);
    printf("Serveur démarré sur le port %d\n", PORT);
    
    ret = listen(sock, 1);
    if (VERBOSE)
    {
        afficherHeure(logs);
        fprintf(logs, "listen=%d\n", ret);
        printf("listen=%d\n", ret);
    }

//----------------------------------------------------------------------------------


    while (1 == 1) {


        size = sizeof(conn_addr);
        cnx = accept(sock, (struct sockaddr *)&conn_addr, (socklen_t *)&size);

        if (VERBOSE)
        {
            afficherHeureIP(logs, conn_addr);
            fprintf(logs, "accept=%d\n", ret);
            printf("accept=%d\n", ret);
        }

        char buffer[BUFFER_SIZE];
        int res;
        char msgClient[100];


        /* //on demande saisir sa clé api
        strcpy(msgClient, "Entrez votre clé API:\n");
        write(cnx, msgClient, strlen(msgClient)); */

        //on lit la requête du client
        res = read(cnx, buffer, BUFFER_SIZE-1);
        buffer[res] = '\0';


        if (VERBOSE)
        {
            afficherHeureIP(logs, conn_addr);
            fprintf(logs, "Requête reçue : %s", buffer);
            printf("Requête reçue : %s", buffer);
        }

        //découpage des arguments de la requête
        char *token = strtok(buffer, "/");
        strcpy(cleAPI, token);

        if(verifCle(token) != true) {
            //si la clé n'est pas bonne
            strcpy(msgClient, "Clé API incorrecte\n\0");

            if (VERBOSE)
            {
                afficherHeureIP(logs, conn_addr);
                fprintf(logs, msgClient);
                printf(msgClient);
            }               
            res = write(cnx, msgClient, strlen(msgClient));
            close(cnx);
        } else {
            //si la clé est bonne
            strcpy(msgClient, "Clé API correcte\n\0");


            if (VERBOSE)
            {
                afficherHeureIP(logs, conn_addr);
                fprintf(logs, msgClient);
                printf("%s", msgClient);
            }               

            res = write(cnx, msgClient, strlen(msgClient));
            token = strtok(NULL, "/");
            
            if(strcmp(token, "\r\n\0") == 0 || strcmp(token, "") == 0) {
                strcpy(buffer, "exit\r\n\0");
                
                if (VERBOSE)
                {
                    afficherHeureIP(logs, conn_addr);
                    strcpy(msgClient, "Requête vide fermeture de la session\n\0");
                    printf("%s", msgClient),
                    fprintf(logs, "%s", msgClient);
                }
                    
            } else if (strcmp(token, "exit\r\n\0") != 0){

                //ecriture de la requete dans le fichier bdd
                afficherHeureIP(logs, conn_addr);
                fprintf(logs, "request(lenght=%d) : %s", res, buffer);
                printf("request(lenght=%d) : %s", res, buffer);

                if (strcmp(token, "getLogement\r\n\0") == 0)
                {
                    getLogement(cleAPI, cnx);
                } else if (strcmp(token, "getCalendrier") == 0)
                {
                    char *id = strtok(NULL, "/");
                    int idLogement = atoi(id);
                    char *dateDebut = strtok(NULL, "/");
                    char *dateFin = strtok(NULL, "/");
                    getCalendrier(cleAPI, cnx, idLogement, dateDebut, dateFin);
                }
                else if (strcmp(token, "miseIndispo") == 0)
                {
                    char *id = strtok(NULL, "/");
                    int idLogement = atoi(id);
                    char *dateDebut = strtok(NULL, "/");
                    char *dateFin = strtok(NULL, "/");
                    miseIndispo(cleAPI, cnx, idLogement, dateDebut, dateFin);
                }
                else if (strcmp(token, "miseDispo") == 0)
                {
                    char *id = strtok(NULL, "/");
                    int idLogement = atoi(id);
                    char *dateDebut = strtok(NULL, "/");
                    char *dateFin = strtok(NULL, "/");
                    miseDispo(cleAPI, cnx, idLogement, dateDebut, dateFin);
                } else if (strcmp(token, "getDispo") == 0)
                {
                    char *id = strtok(NULL, "/");
                    int idLogement = atoi(id);
                    char *dateDebut = strtok(NULL, "/");
                    char *dateFin = strtok(NULL, "/");
                    getDispo(cleAPI, cnx, idLogement, dateDebut, dateFin);
                } else {
                    strcpy(msgClient, "Requête inconnue\n\0");
                    res = write(cnx, msgClient, strlen(msgClient));
                }                                                     
            }
            
            strcpy(msgClient, "Fermeture de la session\n\0");
            res = write(cnx, msgClient, strlen(msgClient));
            close(cnx);

            if (VERBOSE) {
                afficherHeureIP(logs, conn_addr);
                fprintf(logs, "Fermeture de la session\n");
            }
            fclose(logs);
        }
        
    }
}