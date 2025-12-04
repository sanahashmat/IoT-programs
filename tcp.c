
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int socket_desc, new_socket, valread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024] = {0};

    pid_t pid;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Prepare address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(socket_desc, 3) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("Server is ready and listening on port %d...\n", PORT);

    pid = fork();  // Create a new process

    if (pid == 0) {
        // ---------------- CLIENT PROCESS ----------------
        sleep(1);  // Wait a bit to let server start

        int client_sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection failed");
            exit(1);
        }

        char *msg = "Hello from Client!";
        send(client_sock, msg, strlen(msg), 0);
        printf("Client sent: %s\n", msg);

        valread = read(client_sock, buffer, 1024);
        printf("Client received: %s\n", buffer);

        close(client_sock);
    } 
    else {
        // ---------------- SERVER PROCESS ----------------
        addr_size = sizeof(client_addr);
        new_socket = accept(socket_desc, (struct sockaddr *)&client_addr, &addr_size);

        valread = read(new_socket, buffer, 1024);
        printf("Server received: %s\n", buffer);

        char *reply = "Hello from Server!";
        send(new_socket, reply, strlen(reply), 0);
        printf("Server sent: %s\n", reply);

        close(new_socket);
        close(socket_desc);
    }

    return 0;
}
