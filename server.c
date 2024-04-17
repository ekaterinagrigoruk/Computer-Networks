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

    char* token = strtok(buffer, " ");
    token = strtok(NULL, " ");
    char filepath[MAX_BUF];
    sprintf(filepath, "%s%s", ROOT_DIR, token);

    FILE* fp = popen(filepath, "r");
    if (fp == NULL) {
        printf("Failed to execute file\n");
        char response[] = "HTTP/1.1 500 Internal Server Error\nContent-Type: text\n\n";
        send(client_socket, response, strlen(response), 0);
        return;
    }

    char response_header[] = "HTTP/1.1 200 OK\nContent-Type: text\n\n";
    send(client_socket, response_header, strlen(response_header), 0);

    char file_buffer[MAX_BUF];
    int bytes_read;
    while (fgets(file_buffer, sizeof(file_buffer)-1, fp) != NULL) {
        send(client_socket, file_buffer, strlen(file_buffer), 0);
        printf("%s", file_buffer);
    }
    
    //while ((bytes_read = fread(file_buffer, 1, MAX_BUF, fp)) > 0) {
        //send(client_socket, file_buffer, bytes_read, 0);
    //}
    
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

        handle_request(cs);
        
        close(cs);
    }
    close(ss);
    return 0;
}
