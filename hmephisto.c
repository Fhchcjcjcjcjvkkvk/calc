#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

void usage() {
    printf("Usage: hmephisto.exe <hash> -d <type>\n");
    printf("Types: sha256, sha512, md5, sha1\n");
    printf("This program uses a wordlist 'pwd.txt' for decryption.\n");
}

int compare_hash(const char *hash, const char *word, const char *type) {
    unsigned char digest[SHA512_DIGEST_LENGTH];
    char mdString[SHA512_DIGEST_LENGTH*2+1];
    int i;

    if (strcmp(type, "sha256") == 0) {
        SHA256((unsigned char*)word, strlen(word), digest);
        for (i = 0; i < SHA256_DIGEST_LENGTH; ++i)
            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    } else if (strcmp(type, "sha512") == 0) {
        SHA512((unsigned char*)word, strlen(word), digest);
        for (i = 0; i < SHA512_DIGEST_LENGTH; ++i)
            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    } else if (strcmp(type, "md5") == 0) {
        MD5((unsigned char*)word, strlen(word), digest);
        for (i = 0; i < MD5_DIGEST_LENGTH; ++i)
            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    } else if (strcmp(type, "sha1") == 0) {
        SHA1((unsigned char*)word, strlen(word), digest);
        for (i = 0; i < SHA_DIGEST_LENGTH; ++i)
            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    } else {
        return 0;
    }

    return strcmp(hash, mdString) == 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4 || strcmp(argv[2], "-d") != 0) {
        usage();
        return 1;
    }

    const char *hash = argv[1];
    const char *type = argv[3];

    FILE *wordlist = fopen("pwd.txt", "r");
    if (!wordlist) {
        printf("Failed to open wordlist 'pwd.txt'.\n");
        return 1;
    }

    char word[256];
    while (fgets(word, sizeof(word), wordlist)) {
        word[strcspn(word, "\n")] = 0; // Remove newline character
        if (compare_hash(hash, word, type)) {
            printf("Password found: %s\n", word);
            fclose(wordlist);
            return 0;
        }
    }

    printf("Password not found in wordlist.\n");
    fclose(wordlist);
    return 1;
}
