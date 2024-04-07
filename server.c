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

void execute_command(int new_socket, char *command) {
    FILE *fp;
    char buffer[MAX_BUF] = {0};

    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error when executing the command");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, MAX_BUF, fp) != NULL) {
        if (send(new_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Error when sending data");
            exit(EXIT_FAILURE);
        }
    }

    pclose(fp);
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
        execute_command(cs, buffer);
        close(cs);
    }
    close(ss);
    return 0;
}
