#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(){
    int cs = socket(AF_INET, SOCK_STREAM, 0);
    if (cs < 0){
        perror("Can't open socket");
        return 0;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1348);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (connect(cs, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        return -1;
    }
    else{
        printf("Connected to the server\n");
    }

    char client_message[256] = "echo\nabcdef\n";
    char server_message[256];
    send(cs, client_message, strlen(client_message), 0);
    read(cs, server_message, sizeof(server_message));

    printf("%s\n", server_message);

    shutdown(cs, SHUT_RDWR);
    return 0;
}
