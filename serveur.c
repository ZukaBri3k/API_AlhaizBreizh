#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {

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

    do
    {
        res = read(cnx, buffer, BUFFER_SIZE-1);
        buffer[res] = '\0';
        printf("read=%d, msg=%s\n", res, buffer);
    } while (strcmp(buffer, "exit\n\r\0") != 0);
    printf("close\n");

}