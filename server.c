#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUF 1024
#define PORT 8080
#define ROOT_DIR "."

void handle_request(int client_socket) {
    char buffer[MAX_BUF] = {0};
    int valread = read(client_socket, buffer, MAX_BUF);
    if (valread <= 0) {
        printf("Failed to read request\n");
        return;
    }
    printf("Request:\n%s\n", buffer);

    // Extracting requested file path from HTTP request
    char *token = strtok(buffer, " ");
    token = strtok(NULL, " ");
    char filepath[256];
    sprintf(filepath, "%s%s", ROOT_DIR, token);

    // Open the requested file
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open file\n");
        char response[] = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n<html><body><h1>404 Not Found</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
    } else {
        // Sending file content as HTTP response
        char response_header[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
        send(client_socket, response_header, strlen(response_header), 0);
        
        char file_buffer[MAX_BUF];
        int bytes_read;
        while ((bytes_read = read(fd, file_buffer, MAX_BUF)) > 0) {
            send(client_socket, file_buffer, bytes_read, 0);
        }
        close(fd);
    }
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

        handle_request(cs);
        
        close(cs);
    }
    close(ss);
    return 0;
}
