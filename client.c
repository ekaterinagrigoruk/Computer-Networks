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

void receive_file(int socket, char *filename) {
    char buffer[MAX_BUF] = {0};
    int fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    ssize_t n;
    while ((n = recv(socket, buffer, MAX_BUF, 0)) > 0) {
        if (write(fd, buffer, n) != n) {
            perror("write failed");
            exit(EXIT_FAILURE);
        }
    }
    close(fd);
}

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

    char filename[] = "packet.h";
    send(cs, filename, strlen(filename), 0);
    receive_file(cs, "received_file.txt");
    close(cs);

    return 0;
}
