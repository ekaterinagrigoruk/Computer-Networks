#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER 5

void reverseString(char* str, int start, int end) {
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void reverseLinesInString(char* input) {
    int length = strlen(input);
    int start = 0;
    int end = 0;

    for (int i = 0; i < length; i++) {
        if (input[i] == '\n' || i == length - 1) {
            end = i - 1;
            reverseString(input, start, end);
            start = i + 1;
        }
    }
}

int main() {
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0) {
        perror("Can't open socket");
        return -1;
    }
    int sockoptval = 1;
    setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13485);
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

    int cs = accept(ss, (struct sockaddr*)&client, &socklen);
    if (cs < 0) {
        perror("Server accept failed");
        return -1;
    }
    else {
        printf("Server accept the client\n");
    }

    char* client_message = (char*)malloc(sizeof(char*) * MAX_BUFFER);
    int counter = 0;
    while (1) {
        int bytes_read = read(cs, client_message + counter * MAX_BUFFER, MAX_BUFFER * sizeof(char));
        if (bytes_read < 0) {
            perror("Reading error");
            return -1;
        }
        if (bytes_read == MAX_BUFFER) {
            client_message = realloc(client_message, sizeof(client_message) + MAX_BUFFER * sizeof(char));
            counter++;
        }
        else if (bytes_read < MAX_BUFFER) {
            break;
        }
    }
    printf("%s\n", client_message);
    reverseLinesInString(client_message);
    printf("%s\n", client_message);
    send(cs, client_message, strlen(client_message), 0);
    close(cs);
    close(ss);
    return 0;
}