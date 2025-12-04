#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[1024] = {0};
    pid_t pid;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("UDP Server started on port %d...\n", PORT);

    pid = fork();  // Create client in another process

    if (pid == 0) {
        // ---------------- CLIENT ----------------
        sleep(1);  // Let server start first
        struct sockaddr_in servAddr;
        servAddr.sin_family = AF_INET;
        servAddr.sin_port = htons(PORT);
        servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        char *msg = "Hello via UDP!";
        sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
        printf("Client sent: %s\n", msg);

        int len = sizeof(servAddr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&servAddr, &len);
        buffer[n] = '\0';
        printf("Client received: %s\n", buffer);
    } 
    else {
        // ---------------- SERVER ----------------
        int len = sizeof(clientAddr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &len);
        buffer[n] = '\0';
        printf("Server received: %s\n", buffer);

        char *reply = "Hello back from UDP Server!";
        sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&clientAddr, len);
        printf("Server sent: %s\n", reply);
    }

    close(sockfd);
    return 0;
}
