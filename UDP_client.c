#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "packet.h"

#define PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct timeval timeout;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    char message[MAX_SIZE];
    printf("Enter a string: ");
    fgets(message, MAX_SIZE, stdin);

    int num_packets = strlen(message) / PACKET_SIZE + 1;
    Packet* packets = (Packet*)malloc(num_packets * sizeof(Packet));
    char buffer[PACKET_SIZE + 1];
    for (int i = 0; i < num_packets; i++) {
        int start = i * PACKET_SIZE;
        int packetSize = PACKET_SIZE < strlen(message) - start ? PACKET_SIZE : strlen(message) - start;
        memmove(packets[i].data, message + start, packetSize);
        packets[i].data[packetSize] = '\0';
        packets[i].id = i;
    }

    int attempts = 0;
    for (int i = 0; i < num_packets || attempts < 3; i++) {
        sendto(sockfd, &packets[i], sizeof(packets[i]), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        int ack;
        recv(sockfd, &ack, sizeof(int), 0);


        if (ack == i) {
            i = ack;
            attempts = 0;
        }
        else {
            attempts++;
            i--;
        }
    }

    close(sockfd);

    return 0;
}
