// Webserver code for CSCI 304 Computer Networks Assignment
// Lannie Dalton Hough

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

int main(int argc, const char *argv[]) {

    // Create a TCP socket
    int socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDesc == -1) {
        printf("ERROR: Socket creation failed!\n");
        perror("socket() error: ");
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80); // Ensures correct endianness

    int bindResult = bind(socketDesc, (struct sockaddr *)&server, sizeof(server));
    if (bindResult == -1) {
        printf("ERROR: Binding failed!\n");
        perror("bind() error: ");
    }

    listen(socketDesc, 5);

    struct sockaddr_in client; // Holds address of connecting entity
    int clientAddrSize = sizeof(struct sockaddr_in);
    
    // Accept incoming connection
    printf("Waiting for connections...\n");
    int clientSocketDesc = accept(socketDesc, (struct sockaddr *)&client, (socklen_t*)&clientAddrSize);
    if (clientSocketDesc == -1) {
        printf("ERROR: Accepting connection failed!\n");
        perror("accept() error: ");
    }

    char *clientIp = inet_ntoa(client.sin_addr); // Network endian bytes to ip address string
    int clientPort = ntohs(client.sin_port);
    
    

    return 0;
}
