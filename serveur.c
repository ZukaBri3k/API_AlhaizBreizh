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
    int size;
    int cnx;
    struct sockaddr_in conn_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr("site-sae-ubisoufte.bigpapoo.com");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("bind=%d\n", ret);
    ret = listen(sock, 1);
    printf("listen=%d\n", ret);
    size = sizeof(conn_addr);
    cnx = accept(sock, (struct sockaddr *)&conn_addr, (socklen_t *)&size);
    printf("accept=%d\n", ret);

}