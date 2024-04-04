#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "packet.h"

#define PORT 12345
#define MAX_SIZE 1000000

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    Packet packet;
    char buffer[MAX_SIZE];
    int ack;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(client_addr);

    int expected_id = 0;

    while (1) {
        recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, &addr_len);

        if (packet.id == expected_id) {
            strncat(buffer, packet.data, strlen(packet.data));
            ack = packet.id;
            expected_id++;
        }

        send(sockfd, &ack, sizeof(int), 0);

        if (strlen(packet.data) < PACKET_SIZE) {
            break;
        }
    }

    printf("Received: %s\n", buffer);

    close(sockfd);
    return 0;
}
