#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

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
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");
    close(sock);

    return 0;
}
