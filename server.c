#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h> 

int main() {
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0) {
        perror("Can't open socket");
        return -1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1348);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(ss, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Can't bind socket");
        return 0;
    }

    if (listen(ss, 5) != 0) {
        perror("Listen error");
        return -1;
    }
    else {
        printf("Server listening...\n");
    }

    struct sockaddr_in client;
    int socklen = sizeof(client);

    int cs = accept(ss, (struct sockaddr*)&servaddr, &socklen);
    if (cs < 0) {
        perror("Server accept failed");
        return -1;
    }
    else {
        printf("Server accept the client\n");
    }

    char client_message[256];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    recvfrom(cs, &client_message, sizeof(client_message), 0, (struct sockaddr*)&client_addr, &addr_len);

    printf("Client said: %s\n", client_message);

    shutdown(ss, SHUT_RDWR);
    shutdown(cs, SHUT_RDWR);
    return 0;
}