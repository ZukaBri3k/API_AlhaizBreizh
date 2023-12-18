#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {

    int PORT = 8080;

    //traitement des options longues
    int opt;
    int option_index = 0;

    //liste des options longues
    struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
    };


    //parcours de toutes les options
    while((opt = getopt_long(argc, argv, ":p:h", long_options, &option_index)) != -1) 
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
        }
    }

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
    printf("bind=%d\n", ret);
    ret = listen(sock, 1);
    printf("listen=%d\n", ret);
    size = sizeof(conn_addr);
    cnx = accept(sock, (struct sockaddr *)&conn_addr, (socklen_t *)&size);
    printf("accept=%d\n", ret);

    char buffer[BUFFER_SIZE];
    int res;
    printf("Serveur démarré sur le port %d\n", PORT);
    do
    {
        res = read(cnx, buffer, BUFFER_SIZE-1);
        buffer[res] = '\0';
        printf("read=%d, msg=%s", res, buffer);
    } while (strcmp(buffer, "exit\r\n\0") != 0);
    printf("close\n");

}