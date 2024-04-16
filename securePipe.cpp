#include <windows.h>
#include <accctrl.h> // Include this header for EXPLICIT_ACCESS
#include <aclapi.h> // Include this header for SetEntriesInAcl
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <mutex>
#include <sddl.h> // Required for ConvertStringSidToSid

#define PIPE_NAME L"\\\\.\\pipe\\hackthispipe"
#define BUFFER_SIZE 512
#define NUM_CLIENT_THREADS 5

std::mutex g_mutex;

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
    while (true) {
        // Define security attributes
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = nullptr;
        sa.bInheritHandle = FALSE;

        // Convert the string representation of the administrators group SID to a binary SID
        PSID adminSid = nullptr;
        if (!ConvertStringSidToSid(TEXT("S-1-5-32-544"), &adminSid)) {
            std::cerr << "Error converting string SID to binary SID." << std::endl;
            return;
        }

        // Create an ACL with a single ACE that allows only administrators to connect
        EXPLICIT_ACCESS ea;
        ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
        ea.grfAccessPermissions = GENERIC_ALL;
        ea.grfAccessMode = SET_ACCESS;
        ea.grfInheritance = NO_INHERITANCE;
        ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
        ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
        ea.Trustee.ptstrName = (LPTSTR)adminSid;

        PACL acl = nullptr;
        DWORD result = SetEntriesInAcl(1, &ea, nullptr, &acl);
        if (result != ERROR_SUCCESS) {
            std::cerr << "Error setting ACE in ACL: " << result << std::endl;
            LocalFree(adminSid);
            return;
        }

        // Create a security descriptor that allows only administrators to connect
        SECURITY_DESCRIPTOR sd;
        if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
            std::cerr << "Error initializing security descriptor." << std::endl;
            LocalFree(adminSid);
            LocalFree(acl);
            return;
        }

        if (!SetSecurityDescriptorDacl(&sd, TRUE, acl, FALSE)) {
            std::cerr << "Error setting security descriptor DACL." << std::endl;
            LocalFree(adminSid);
            LocalFree(acl);
            return;
        }

        sa.lpSecurityDescriptor = &sd;

        // Create named pipe with the specified security attributes
        HANDLE hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_OUTBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            BUFFER_SIZE,
            BUFFER_SIZE,
            0,
            &sa
        );

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Error creating named pipe." << std::endl;
            LocalFree(adminSid);
            LocalFree(acl);
            return;
        }

        // Wait for client connection
        if (!ConnectNamedPipe(hPipe, nullptr)) {
            CloseHandle(hPipe);
            continue;
        }

        // Send data to client
        std::string dummyID = "TP061084";
        std::string dummypassword = "Danisy123@";
        std::string deviceInfo = getDeviceInfo(dummyID, dummypassword);
        DWORD bytesWritten;
        WriteFile(hPipe, deviceInfo.c_str(), deviceInfo.length(), &bytesWritten, nullptr);
        std::cout << "\nData sent to client" << std::endl;
        // Close pipe
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);

        // Free resources
        LocalFree(adminSid);
        LocalFree(acl);
    }
}

void PipeClientThread() {
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


    

    // Close pipe
    CloseHandle(hPipe);
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

    std::vector<std::thread> clientThreads;
    for (int i = 0; i < NUM_CLIENT_THREADS; ++i) {
        clientThreads.emplace_back(PipeClientThread);
    }

    serverThread.join();
    for (auto& thread : clientThreads) {
        thread.join();
    }

    return 0;
}
