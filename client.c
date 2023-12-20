#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

// Définir une structure pour les informations du client
typedef struct {
    char username[50];
    char password[50];
} ClientInfo;

typedef struct {
    int command;
    union {
        ClientInfo clientInfo;
    } data;
} ClientCommand;

int main(int argc, char* argv[]) {
    int PORT = 8080;
    int sock;
    int ret;
    int cnx;
    struct sockaddr_in addr;

    //traitement des options longues
    int opt;
    int option_index = 0;

    //liste des options longues
    struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},
    };

    //parcours de toutes les options
    while((opt = getopt_long(argc, argv, ":p:m", long_options, &option_index)) != -1) { 
        switch(opt) { 
            case 'p': 
                printf("Port personnalisé : %s\n", optarg);
                PORT = atoi(optarg);
                break;
        }
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr("51.77.222.112");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    cnx = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("%d\n", cnx);
    ClientCommand command;
    printf("Veuillez entrer votre nom d'utilisateur : ");
    scanf("%s", command.data.clientInfo.username);
    printf("Veuillez entrer votre mot de passe : ");
    scanf("%s", command.data.clientInfo.password);
    command.command = 1;
    send(sock, &command, sizeof(command), 0);
    close(sock);

    return 0;
}
