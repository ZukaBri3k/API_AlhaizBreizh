#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

int main() {
    int sock;
    int ret;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Serveur démarré sur le port %d\n", PORT);

    while (1) {
        ret = listen(sock, 1);
        if (ret == -1) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        printf("En attente de connexion...\n");

        struct sockaddr_in conn_addr;
        socklen_t size = sizeof(conn_addr);
        int cnx = accept(sock, (struct sockaddr *)&conn_addr, &size);
        if (cnx == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Client connecté\n");
        close(cnx);
    }
    return 0;
}