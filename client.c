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
    ServerResponse response;
    recv(sock, &response, sizeof(response), 0);
    if (response.status == 0) {
        switch (command.command) {
            case 1:
                printf("Opération réussie\n");
                break;

            default:
                printf("Réponse inattendue du serveur.\n");
                break;
        }
    } else {
        printf("Erreur du serveur. Code d'erreur : %d\n", response.status);
        close(sock);
    
    printf("Que souhaitez-vous faire ?\n");
    printf("1. Consulter la liste des biens\n");
    printf("2. Consulter la disponibilité d'un bien\n");
    printf("3. Modifier les dates d'indisponibilité d'un bien\n");
    printf("4. Consulter la liste de tous les biens (admin)\n");
    printf("0. Quitter\n");
    printf("Votre choix : ");
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        command.command = 2;
        send(sock, &command, sizeof(command), 0);
        recv(sock, &response, sizeof(response), 0);
        if (response.status == 0) {
            printf("Liste des biens du client :\n");
            printf(response);
        } else {
            printf("Erreur du serveur lors de la consultation de la liste des biens. Code d'erreur : %d\n", response.status);
        }
    } else if (choix == 2) {
        command.command = 3;
        send(sock, &command, sizeof(command), 0);
        recv(sock, &response, sizeof(response), 0);
        if (response.status == 0) {
            printf("Disponibilité du bien :\n");
            printf(response);
        } else {
            printf("Erreur du serveur lors de la consultation de la disponibilité d'un bien. Code d'erreur : %d\n", response.status);
        }
    } else if (choix == 3) {
        command.command = 4;
        send(sock, &command, sizeof(command), 0);
        recv(sock, &response, sizeof(response), 0);
        if (response.status == 0) {
            printf("Dates d'indisponibilité modifiées avec succès.\n");
            printf(response);
        } else {
            printf("Erreur du serveur lors de la modification des dates d'indisponibilité. Code d'erreur : %d\n", response.status);
        }
    } else if (choix == 4) {
        command.command = 5;
        send(sock, &command, sizeof(command), 0);
        recv(sock, &response, sizeof(response), 0);
        if (response.status == 0) {
            printf("La liste de tous les biens :\n");
            printf(response);
        } else {
            printf("Erreur du serveur lors de la consultation de tous les biens. Code d'erreur : %d\n", response.status);
        }
    } else {
        printf("Opération terminée.\n");
    }
    return 0;
}
