#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#define PORT 8000
int main() {
    int sock;
    int ret;
    struct sockaddr_in addr;
        sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    char *hostname = "site-sae-ubisoufte.bigpapoo.com";
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL) {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        fprintf(stderr, "connect: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    char response[1024];
    int len;

    len = read(sock, buffer, sizeof(buffer));
    buffer[len] = '\0';

    printf("client : %s", buffer);

    scanf("%s", buffer);
    write(sock, buffer, strlen(buffer));

    len = read(sock, buffer, sizeof(buffer));
    buffer[len] = '\0';

    printf("client : %s\n", buffer);

    int choix;
    choix = 1;
    do {
        printf("Que souhaitez-vous faire ?\n");
        printf("1. Consulter la liste des biens\n");
        printf("2. Consulter la disponibilité d'un bien\n");
        printf("3. Modifier les dates d'indisponibilité d'un bien\n");
        printf("4. Consulter la liste de tous les biens (clé privilégiée)\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        //if (choix == 1) {
        //    send(sock, &command, sizeof(command), 0);
        //    recv(sock, &response, sizeof(response), 0);
        //    printf("Liste des biens du client :\n");
        //    printf("%s", response);
        //}else if (choix == 2) {
        //    send(sock, &command, sizeof(command), 0);
        //    recv(sock, &response, sizeof(response), 0);
        //    printf("Disponibilité du bien :\n");
        //    printf("%s", response);
        //}else if (choix == 3) {
        //    send(sock, &command, sizeof(command), 0);
        //    recv(sock, &response, sizeof(response), 0);
        //    printf("Dates d'indisponibilité modifiées avec succès.\n");
        //    printf("%s", response);
        /*}else*/
        if (choix == 4) {
            printf("client : Choix 4\n");
            int res = write(sock, "getLogement\r\n\0", sizeof("getLogement\r\n\0"));
            printf("client : res = %d\n", res);

            printf("Liste de tous les biens :\n");
            /*len = read(sock, buffer, sizeof(buffer));
            buffer[len] = '\0';*/

            do
            {
                len = read(sock, buffer, sizeof(buffer));
                printf("client : len = %d\n", len);
                buffer[len] = '\0';
                printf("%s\n", buffer);
            } while (len > 0);
            printf("%s\n", buffer);
            

        } else if (choix == 0) {
            printf("Opération terminée.\n");
            close(sock);
        }else {
            printf("Choix non reconnu. Veuillez entrer un nombre entre 1 et 4.\n");
        }
    } while (choix != 0);
    return 0;
}