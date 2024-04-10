#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX_BUF 1024
#define PORT 8080

int main() {
    int cs = socket(AF_INET, SOCK_STREAM, 0);
    if (cs < 0){
        perror("Can't open socket");
        return 0;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (connect(cs, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        return -1;
    }
    else{
        printf("Connected to the server\n");
    }

    char request[] = "GET /packet.h HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send(cs, request, strlen(request), 0);

    char buffer[MAX_BUF];
    int valread = read(cs, buffer, MAX_BUF);
    if (valread < 0) {
        perror("Reading response failed");
        exit(EXIT_FAILURE);
    }
    printf("Response:\n%s\n", buffer);
    
    close(cs);

    return 0;
}
