#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int sock;
    int ret;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    cnx = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
}