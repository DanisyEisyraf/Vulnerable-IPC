#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

#define PIPE_NAME L"\\\\.\\pipe\\hackthispipe"
#define BUFFER_SIZE 512

void SetConsoleColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

std::string getDeviceInfo(const std::string& tpNo, const std::string& password) {
    // Get device name
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerNameA(computerName, &size);

    // Get process ID
    DWORD pid = GetCurrentProcessId();
    std::stringstream ss;
    ss << pid;
    std::string pid_str = ss.str();

    // Get current time
    time_t now = time(0);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);
    char timeStr[26];
    asctime_s(timeStr, sizeof(timeStr), &timeInfo);

    // Format the information
    std::string deviceInfo = tpNo + "|" + std::string(computerName) + "|" + pid_str + "|" + std::string(timeStr) + "|Active";

    return deviceInfo;
}


void PipeServerThread() {
    HANDLE hPipe;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    while (true) {
        // Create named pipe
        hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_OUTBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            BUFFER_SIZE,
            BUFFER_SIZE,
            0,
            nullptr
        );

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Error creating named pipe." << std::endl;
            return;
        }

        // Wait for client connection
        if (!ConnectNamedPipe(hPipe, nullptr)) {
            CloseHandle(hPipe);
            continue;
        }
        // Send data to client
        // Substitute this section with getinfoDevice().
        std::string dummyID;
        std::string dummypassword;
        dummyID = "Enter Dummy ID HERE";
        dummypassword = "Dummy Password here";
        std::string deviceInfo = getDeviceInfo(dummyID, dummypassword);
        WriteFile(hPipe, deviceInfo.c_str(), deviceInfo.length(), &bytesRead, nullptr);

        // Close pipe
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }
}

int main() {

    std::string TPNo;
    std::string password;

    SetConsoleColor(FOREGROUND_BLUE);

    std::cout << "              _                      _     _            __             _            ___                 " << std::endl;
    std::cout << " /\\   /\\_   _| |_ __   ___ _ __ __ _| |__ | | ___      / /  ___   __ _(_)_ __      / _ \\__ _  __ _  ___ " << std::endl;
    std::cout << " \\ \\ / | | | | | '_ \\ / _ | '__/ _` | '_ \\| |/ _ \\    / /  / _ \\ / _` | | '_ \\    / /_)/ _` |/ _` |/ _ \\ " << std::endl;
    std::cout << "  \\ V /| |_| | | | | |  __| | | (_| | |_) | |  __/   / /__| (_) | (_| | | | | |  / ___| (_| | (_| |  __/ " << std::endl;
    std::cout << "   \\_/  \\__,_|_|_| |_|\\___|_|  \\__,_|_.__/|_|\\___|   \\____/\\___/ \\__, |_|_| |_|  \\/    \\__,_|\\__, |\\___| " << std::endl;
    std::cout << "                                                                 |___/                       |___/      " << std::endl;
    std::cout << "\n\n\n\n\n";
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    std::cout << "Enter your TP Number: ";
    std::cin >> TPNo;
    std::cout << "Enter your password: ";
    std::cin >> password;

    //SQL Injection command: admin' OR '1'='1'--
    std::string dataToSend = TPNo + "|" + password;
    std::string deviceInfo = getDeviceInfo(TPNo, password);

    // Start pipe server thread
    std::thread serverThread(PipeServerThread);
    // Create multiple client threads
    std::vector<std::thread> clientThreads;
    for (int i = 0; i < 5; ++i) {
        clientThreads.emplace_back([]() {
            HANDLE hPipe;
            char buffer[BUFFER_SIZE];
            DWORD bytesRead;

            hPipe = CreateFile(
                PIPE_NAME,
                GENERIC_READ,
                0,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr
            );
            if (hPipe == INVALID_HANDLE_VALUE) {
                std::cerr << "Error connecting to named pipe." << std::endl;
                return;
            }
            // Close pipe
            CloseHandle(hPipe);
            });
    }
    // Join client threads
    for (auto& thread : clientThreads) {
        thread.join();
    }
    // Join server thread
    serverThread.join();
    return 0;
}
