// Webserver code for CSCI 304 Computer Networks Assignment
// Lannie Dalton Hough

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int parseRequest(char *req, int socket) {
    printf("%s", req);
    // Get first line of req
    char *getReqLine = strtok(req, "\n");
    //printf("%s\n", getReqLine);

    // Get first two elements of line, should be GET & a path
    char *tokens[2];
    char *token = strtok(getReqLine, " ");
    int i = 0;
    while ((token != NULL) && (i < 2)) {
        tokens[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    if (0 != strcmp(tokens[0], "GET")) { // Only supporting GET rn
        printf("Req is not GET req.");
        write(socket, "Error!", strlen("Error!"));
        return -1;
    }

    char *notFound = "HTTP/1.1 404 Not found\nContent-Type: text/html\nContent-Length: 19\n\n<h1>404 Error</h1>\n";

    // Attempt to prevent path traversal
    char *badPath = "..";
    if (strstr(tokens[1], badPath) != NULL) {
        printf("Found .. in passed path.\n");
        write(socket, notFound, strlen(notFound));
        return -1;
    }

    // Attempt to read requested file
    FILE *file;
    int pathLen = strlen(tokens[1])+1;
    char *path = calloc(1, strlen(tokens[1])+1);
    strncat(path, ".", strlen("."));
    strncat(path, tokens[1], strlen(tokens[1]));
    if (file = fopen(path, "r")) {
        printf("File found!\n");
        
        // Get size of file
        long size;
        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        rewind(file);
        
        char *buffer;
        buffer = calloc(1, size+1);

        // Read file
        fread(buffer, size, 1, file); 
        fclose(file);

        char *response;
        response = calloc(1, size+1000); // Make sure big enough for content & headers
        char *res_1 = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        int len = snprintf(NULL, 0, "%d", size);
        char *contentLengthHeader = malloc(len + 1);
        snprintf(contentLengthHeader, len+1, "%d", size);
        char *newLines = "\n\n";
        strncat(response, res_1, 100);
        strncat(response, contentLengthHeader, 100);
        strncat(response, newLines, 100);
        strncat(response, buffer, size+100);

        write(socket, response, strlen(response));

        free(contentLengthHeader);
        free(response);
        free(buffer);
        free(path);
        return 0;
    } else {
        printf("File not found!\n");
        write(socket, notFound, strlen(notFound));
        free(path);
        return -1;
    }
    return 0;
}


void *threadSocket(void *arg) {
    printf("Created a thread!\n");
    int clientSocket = *((int *)arg);
    if (clientSocket == -1) {
        printf("ERROR: Accepting connection failed\n");
        perror("accept() error: ");
    }
    char buffer[4096];
    // Read the req from client
    ssize_t msgSize = recv(clientSocket, &buffer, sizeof(buffer), 0);
    parseRequest(buffer, clientSocket);
    close(clientSocket);
}


int main(int argc, const char *argv[]) {

    // Create a TCP socket
    int socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDesc == -1) {
        printf("ERROR: Socket creation failed!\n");
        perror("socket() error: ");
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80); // Ensures correct endianness

    int bindResult = bind(socketDesc, (struct sockaddr *)&server, sizeof(server));
    if (bindResult == -1) {
        printf("ERROR: Binding failed!\n");
        perror("bind() error: ");
        return -1;
    }

    listen(socketDesc, 50);

    struct sockaddr_in client; // Holds address of connecting entity
    int clientAddrSize = sizeof(struct sockaddr_in);
    
    // Accept incoming connection
    printf("Waiting for connections...\n");
    pthread_t tid[60];
    int i = 0;
    while (1) {
        int clientSocketDesc = accept(socketDesc, (struct sockaddr *)&client, (socklen_t*)&clientAddrSize);
        if (pthread_create(&tid[i++], NULL, threadSocket, &clientSocketDesc) != 0)
            printf("Failed to create a thread!\n");
        if (i >= 50) {
            i = 0;
            while (i < 50) {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
    }

    char *clientIp = inet_ntoa(client.sin_addr); // Network endian bytes to ip address string
    int clientPort = ntohs(client.sin_port);
    
    return 0;
}
