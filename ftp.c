#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 21
#define BUFFER_SIZE 1024

// FTP login credentials
#define USERNAME "ADMIN"
#define PASSWORD "password123"

// Initialize Winsock
void initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Winsock initialization failed\n");
        exit(1);
    }
}

// Start the FTP server
void startFTPServer() {
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Binding failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        perror("Listen failed");
        exit(1);
    }
    printf("FTP server started on port %d\n", PORT);

    // Accept client connection
    if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize)) == INVALID_SOCKET) {
        perror("Client connection failed");
        exit(1);
    }
    printf("Client connected\n");

    // Send welcome message
    send(clientSocket, "220 Welcome to FTP server\r\n", 25, 0);

    // Handle login
    int loggedIn = 0;
    while (!loggedIn) {
        // Receive username
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesReceived] = '\0';

        if (strstr(buffer, "USER ") == buffer) {
            send(clientSocket, "331 Username OK, need password\r\n", 32, 0);
            memset(buffer, 0, sizeof(buffer));

            // Receive password
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            if (strstr(buffer, "PASS ") == buffer && strstr(buffer, PASSWORD) != NULL) {
                send(clientSocket, "230 Login successful\r\n", 23, 0);
                loggedIn = 1;
            } else {
                send(clientSocket, "530 Login incorrect\r\n", 22, 0);
            }
        }
    }

    // Handle FTP commands after login (basic response)
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesReceived] = '\0';

        if (strstr(buffer, "QUIT") == buffer) {
            send(clientSocket, "221 Goodbye\r\n", 13, 0);
            break;
        } else {
            send(clientSocket, "502 Command not implemented\r\n", 28, 0);
        }
    }

    // Close the connection
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    initWinsock();
    startFTPServer();
    return 0;
}
