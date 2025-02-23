#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/blake2.h>
#include <windows.h>

#define WORDLIST_FILE "pwd.txt"
#define HASH_BUFFER_SIZE 64

void usage(const char *progname) {
    fprintf(stderr, "Usage: %s <hash> -d <hash_type>\n", progname);
    fprintf(stderr, "Supported hash types: sha256, sha512, md5, blake2b, sha1\n");
}

int compare_hash(const char *hash, const char *computed_hash, int hash_len) {
    return strncmp(hash, computed_hash, hash_len) == 0;
}

void compute_hash(const char *input, char *output, const char *hash_type) {
    unsigned char hash[HASH_BUFFER_SIZE];
    unsigned int length = 0;

    if (strcmp(hash_type, "sha256") == 0) {
        length = SHA256_DIGEST_LENGTH;
        SHA256((unsigned char *)input, strlen(input), hash);
    } else if (strcmp(hash_type, "sha512") == 0) {
        length = SHA512_DIGEST_LENGTH;
        SHA512((unsigned char *)input, strlen(input), hash);
    } else if (strcmp(hash_type, "md5") == 0) {
        length = MD5_DIGEST_LENGTH;
        MD5((unsigned char *)input, strlen(input), hash);
    } else if (strcmp(hash_type, "blake2b") == 0) {
        length = BLAKE2B_OUTBYTES;
        BLAKE2b(hash, BLAKE2B_OUTBYTES, input, strlen(input), NULL, 0);
    } else if (strcmp(hash_type, "sha1") == 0) {
        length = SHA_DIGEST_LENGTH;
        SHA1((unsigned char *)input, strlen(input), hash);
    }

    for (unsigned int i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[length * 2] = '\0';
}

int main(int argc, char **argv) {
    if (argc != 4 || strcmp(argv[2], "-d") != 0) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *hash = argv[1];
    const char *hash_type = argv[3];
    char computed_hash[HASH_BUFFER_SIZE * 2 + 1];
    char word[256];
    FILE *wordlist = fopen(WORDLIST_FILE, "r");

    if (!wordlist) {
        perror("Error opening wordlist file");
        return EXIT_FAILURE;
    }

    while (fgets(word, sizeof(word), wordlist)) {
        word[strcspn(word, "\r\n")] = '\0'; // Remove newline characters
        compute_hash(word, computed_hash, hash_type);

        if (compare_hash(hash, computed_hash, strlen(hash))) {
            printf("Password found: %s\n", word);
            fclose(wordlist);
            return EXIT_SUCCESS;
        }
    }

    printf("Password not found in wordlist\n");
    fclose(wordlist);
    return EXIT_FAILURE;
}
