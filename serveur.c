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

int main(int argc, char* argv[]) {

    int PORT = 8080;
    bool VERBOSE = false;
    FILE *logs = fopen("logs.txt", "a");

    //declaration des tubes
    int serveur2bdd = open("serveur2bdd", O_WRONLY);
    int bdd2serveur = open("bdd2serveur", O_RDONLY);
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
    
    while (1 == 1) {

        ret = listen(sock, 1);

        if (VERBOSE)
        {
            afficherHeure(logs);
            fprintf(logs, "listen=%d\n", ret);
            printf("listen=%d\n", ret);
        }

        size = sizeof(conn_addr);
        cnx = accept(sock, (struct sockaddr *)&conn_addr, (socklen_t *)&size);

        if (VERBOSE)
        {
            afficherHeure(logs);
            fprintf(logs, "accept=%d\n", ret);
            printf("accept=%d\n", ret);
        }

        char buffer[BUFFER_SIZE];
        int res;
        char msgClient[100];

            //on demande saisir sa clé api
            strcpy(msgClient, "Entrez votre clé API:\n");
            write(cnx, msgClient, strlen(msgClient));

            if (VERBOSE)
            {
                afficherHeure(logs);
                fprintf(logs, msgClient);
            }

            //on lit la clé api
            res = read(cnx, buffer, BUFFER_SIZE-1);
            buffer[res] = '\0';

            if (VERBOSE)
            {
                afficherHeure(logs);
                fprintf(logs, "Clé API saisie : %s", buffer);
                printf("Clé API saisie : %s", buffer);
            }
            res = write(serveur2bdd, buffer, strlen(buffer));
            res = read(bdd2serveur, buffer, BUFFER_SIZE-1);
            buffer[res] = '\0';   

            if(strcmp(buffer, "true\0") != 0) {
                //si la clé n'est pas bonne
                strcpy(msgClient, "Clé API incorrecte fermeture de la session\n\0");

                if (VERBOSE)
                {
                    afficherHeure(logs);
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
                    afficherHeure(logs);
                    fprintf(logs, msgClient);
                    printf(msgClient);
                }               
                res = write(cnx, msgClient, strlen(msgClient));
                
                do {
                    res = read(cnx, buffer, BUFFER_SIZE-1);
                    buffer[res] = '\0';

                    if(strcmp(buffer, "\r\n\0") == 0 || strcmp(buffer, "") == 0) {
                        strcpy(buffer, "exit\r\n\0");
                        
                        if (VERBOSE)
                        {
                            afficherHeure(logs);
                            strcpy(msgClient, "Requête vide fermeture de la session\n\0");
                            printf("%s", msgClient),
                            fprintf(logs, "%s", msgClient);
                        }
                        
                    } else {
                        if (VERBOSE)
                        {
                            afficherHeure(logs);
                            fprintf(logs, "request(lenght=%d) : %s", res, buffer);
                            printf("request(lenght=%d) : %s", res, buffer);
                        }
                    }
                    
                } while (strcmp(buffer, "exit\r\n\0") != 0);
                
                strcpy(msgClient, "Fermeture de la session\n\0");
                res = write(cnx, msgClient, strlen(msgClient));
                close(cnx);

                if (VERBOSE) {
                    afficherHeure(logs);
                    fprintf(logs, "Fermeture de la session\n");
                }
                fclose(logs);
            }
        
    }
}