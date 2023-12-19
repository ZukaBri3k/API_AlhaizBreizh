#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

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
    while((opt = getopt_long(argc, argv, ":p", long_options, &option_index)) != -1) 
    { 
        switch(opt)
        { 
            case 'p': 
                printf("Port personnalisé : %s\n", optarg);
                PORT = atoi(optarg);
                break;
        }
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    cnx = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    return 0;
}
