#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wlanapi.h>
#include <pcap.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "wpcap.lib")

void connect_to_wifi(const char *ssid, const char *password) {
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    WLAN_INTERFACE_INFO_LIST *pIfList = NULL;
    WLAN_INTERFACE_INFO *pIfInfo = NULL;
    WLAN_CONNECTION_PARAMETERS wlanConnParams;
    WLAN_AVAILABLE_NETWORK_LIST *pBssList = NULL;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        printf("Error opening handle: %u\n", dwResult);
        return;
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        printf("Error enumerating interfaces: %u\n", dwResult);
        WlanCloseHandle(hClient, NULL);
        return;
    }

    pIfInfo = &pIfList->InterfaceInfo[0];

    dwResult = WlanScan(hClient, &pIfInfo->InterfaceGuid, NULL, NULL, NULL);
    if (dwResult != ERROR_SUCCESS) {
        printf("Error scanning: %u\n", dwResult);
        WlanFreeMemory(pIfList);
        WlanCloseHandle(hClient, NULL);
        return;
    }

    dwResult = WlanGetAvailableNetworkList(hClient, &pIfInfo->InterfaceGuid, 0, NULL, &pBssList);
    if (dwResult != ERROR_SUCCESS) {
        printf("Error getting network list: %u\n", dwResult);
        WlanFreeMemory(pIfList);
        WlanCloseHandle(hClient, NULL);
        return;
    }

    for (int i = 0; i < (int)pBssList->dwNumberOfItems; i++) {
        if (strcmp((char *)pBssList->Network[i].dot11Ssid.ucSSID, ssid) == 0) {
            wlanConnParams.wlanConnectionMode = wlan_connection_mode_profile;
            wlanConnParams.strProfile = ssid;
            wlanConnParams.pDot11Ssid = NULL;
            wlanConnParams.pDesiredBssidList = NULL;
            wlanConnParams.dot11BssType = dot11_BSS_type_any;
            wlanConnParams.dwFlags = 0;

            dwResult = WlanConnect(hClient, &pIfInfo->InterfaceGuid, &wlanConnParams, NULL);
            if (dwResult != ERROR_SUCCESS) {
                printf("Error connecting: %u\n", dwResult);
            } else {
                printf("Connected to %s\n", ssid);
            }

            break;
        }
    }

    WlanFreeMemory(pBssList);
    WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);
}

void capture_packets(const char *ssid) {
    pcap_if_t *alldevs;
    pcap_if_t *d;
    pcap_t *adhandle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fcode;
    char packet_filter[] = "ip and tcp";

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return;
    }

    for (d = alldevs; d != NULL; d = d->next) {
        if (strstr(d->name, ssid)) {
            adhandle = pcap_open_live(d->name, 65536, 1, 1000, errbuf);
            if (adhandle == NULL) {
                fprintf(stderr, "Unable to open the adapter: %s\n", errbuf);
                pcap_freealldevs(alldevs);
                return;
            }

            if (pcap_compile(adhandle, &fcode, packet_filter, 1, 0xffffff00) < 0) {
                fprintf(stderr, "Unable to compile the packet filter: %s\n", pcap_geterr(adhandle));
                pcap_close(adhandle);
                pcap_freealldevs(alldevs);
                return;
            }

            if (pcap_setfilter(adhandle, &fcode) < 0) {
                fprintf(stderr, "Error setting the filter: %s\n", pcap_geterr(adhandle));
                pcap_close(adhandle);
                pcap_freealldevs(alldevs);
                return;
            }

            char filename[256];
            snprintf(filename, sizeof(filename), "%s.pcap", ssid);
            pcap_dumper_t *dumpfile = pcap_dump_open(adhandle, filename);
            if (dumpfile == NULL) {
                fprintf(stderr, "Error opening output file: %s\n", pcap_geterr(adhandle));
                pcap_close(adhandle);
                pcap_freealldevs(alldevs);
                return;
            }

            printf("Capturing packets on %s...\n", d->name);
            pcap_loop(adhandle, 0, pcap_dump, (unsigned char *)dumpfile);

            pcap_dump_close(dumpfile);
            pcap_close(adhandle);
            break;
        }
    }

    pcap_freealldevs(alldevs);
}

int main() {
    char ssid[256];
    char password[256];

    printf("ENTER SSID: ");
    scanf("%255s", ssid);
    printf("ENTER PWD: ");
    scanf("%255s", password);

    connect_to_wifi(ssid, password);
    capture_packets(ssid);

    return 0;
}
