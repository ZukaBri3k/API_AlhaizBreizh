#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void readReadmeFile() {
    FILE *file;
    char ch;

    // Ouvrir le fichier README en mode lecture
    file = fopen("README.md", "r");

    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier README.md");
        exit(EXIT_FAILURE);
    }

    // Lire et afficher le contenu du fichier
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // Fermer le fichier
    fclose(file);
}

int main() {
    int sock;
    int ret;
    int cnx;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    cnx = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    readReadmeFile();
    return 0;
}
