#pragma once
#define MAX_SIZE 1000000
#define PACKET_SIZE 10

typedef struct {
    int id;
    char data[PACKET_SIZE];
} Packet;