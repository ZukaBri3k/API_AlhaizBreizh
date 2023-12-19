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
    while((opt = getopt_long(argc, argv, ":p", long_options, &option_index)) != -1) 
    { 
        switch(opt)
        { 
            case 'p': 
                //si une option de port est donnée
                printf("port: %s\n", optarg);
                PORT = atoi(optarg);
                break;
            case 'h':
                //renvoie l'aide
                printf("help\n");
                break;
            case 'v':
                printf("Verbose activé\n");
                VERBOSE = true;
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
    (VERBOSE) ? (afficherHeure(logs), fprintf(logs, "bind=%d\n", ret), printf("bind=%d\n", ret)) : 0;  

    //affichage du démarrage
    afficherHeure(logs);
    fprintf(logs, "Serveur démarré sur le port %d\n", PORT);
    printf("Serveur démarré sur le port %d\n", PORT);
    
    while (1 == 1) {

        ret = listen(sock, 1);
        (VERBOSE) ? (afficherHeure(logs), fprintf(logs, "listen=%d\n", ret), printf("listen=%d\n", ret)) : 0;

        size = sizeof(conn_addr);
        cnx = accept(sock, (struct sockaddr *)&conn_addr, (socklen_t *)&size);
        (VERBOSE) ? (afficherHeure(logs), fprintf(logs, "accept=%d\n", ret), printf("accept=%d\n", ret)) : 0;

        char buffer[BUFFER_SIZE];
        int res;

        do
        {
            res = read(cnx, buffer, BUFFER_SIZE-1);
            buffer[res] = '\0';
            (strcmp(buffer, "\0") == 0) ? strcpy(buffer, "exit\r\n\0") : 0;

            (VERBOSE) ? (afficherHeure(logs), fprintf(logs, "request(lenght=%d) : %s", res, buffer), printf("request(lenght=%d) : %s", res, buffer)) : 0;
        } while (strcmp(buffer, "exit\r\n\0") != 0);
        
    }
    
    //affichage de fermeture du serveur
    afficherHeure(logs);
    fprintf(logs, "close\n");
    printf("close\n");
    fclose(logs);

}