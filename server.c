#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUF 1024
#define PORT 8080

void send_file(int new_socket, char *filename) {
    char buffer[MAX_BUF] = {0};
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    ssize_t n;
    while ((n = read(fd, buffer, MAX_BUF)) > 0) {
        if (send(new_socket, buffer, n, 0) == -1) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }
    close(fd);
}

int main() {
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0){
        perror("Can't open socket");
        return -1;
    }
    int sockoptval = 1;
    setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(ss, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
        perror("Can't bind socket");
        return 0;
    }

    if(listen(ss, 5) != 0){
        perror("Listen error");
        return -1;
    }
    else{
        printf("Server listening...\n");
    }

    struct sockaddr_in client;
    int socklen = sizeof(client);
    char buffer[MAX_BUF] = {0};

    while(1) {
        int cs = accept(ss, (struct sockaddr*)&client, &socklen);
        if(cs < 0){
            perror("Server accept failed");
            return -1;
        }
        else{
            printf("Server accept the client\n");
        }

        read(cs, buffer, MAX_BUF);
        printf("%s\n", buffer);
        send_file(cs, "packet.h");
        close(cs);
    }

    return 0;
}
