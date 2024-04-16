#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <iostream>
#include <sys/types.h>
#include <ctime>
#include <sstream>

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




    // Named Pipe Local Variable
    HANDLE hCreateNamedPipe;
    char szInputBuffer[1023];
    char szOutputBuffer[1023];
    DWORD dwszInputBuffer = sizeof(szInputBuffer);
    DWORD dwszOutputBuffer = sizeof(szOutputBuffer);

    // ConnectNamedPipe Variable
    BOOL bConnectNamedPipe;

    // CreateNamedPipe
    hCreateNamedPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\hackthispipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        dwszOutputBuffer,
        dwszInputBuffer,
        0,
        NULL
    );

    if (hCreateNamedPipe == INVALID_HANDLE_VALUE) {
        SetConsoleColor(FOREGROUND_RED);
        std::cout << "NamedPipe creation failed. Error code: " << GetLastError() << std::endl;
    }
    else {
        // Print success message in green
        SetConsoleColor(FOREGROUND_GREEN);
        std::cout << "Pipe creation successful." << std::endl;
    }

//////////////////////////////Calling the Client_Handler.py////////////////////////
    // Path to the Python interpreter
    std::wstring pythonInterpreter = L"python";
    std::wstring pythonScript = L"Insert Directory Here";
    std::wstring commandLineArgs = L"";
    // Create process parameters
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    // Initialize structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Concatenate command line
    std::wstring command = pythonInterpreter + L" " + pythonScript + L" " + commandLineArgs;

    // Create process
    if (!CreateProcessW(NULL,   // No module name (use command line)
        const_cast<LPWSTR>(command.c_str()), // Command line
        NULL,       // Process handle not inheritable
        NULL,       // Thread handle not inheritable
        FALSE,      // Set handle inheritance to FALSE
        0,          // No creation flags
        NULL,       // Use parent's environment block
        NULL,       // Use parent's starting directory
        &si,        // Pointer to STARTUPINFO structure
        &pi)        // Pointer to PROCESS_INFORMATION structure
        ) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return 1;
    }
/////////////////////////End of Calling/////////////////////////////


    std::cout << "Connecting to pipe..." << std::endl;
    bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);

    if (bConnectNamedPipe == FALSE) {
        SetConsoleColor(FOREGROUND_RED);
        std::cout << "Failed to connect to pipe. Error no: " << GetLastError() << std::endl;
    }
    else {
        SetConsoleColor(FOREGROUND_GREEN);
        std::cout << "Successfully connected to Pipe" << std::endl;
    }


    ////////////////////////////////////////////PIPE DATA SENDING//////////////////////
    // Write to Pipe
    DWORD dwNoBytesWrite;
    BOOL bWriteFile = WriteFile(
        hCreateNamedPipe,
        dataToSend.c_str(),
        dataToSend.length(),
        &dwNoBytesWrite,
        NULL
    );
    if (bWriteFile == FALSE) {
        // Print error message in red
        SetConsoleColor(FOREGROUND_RED);
        std::cout << "Failed to write into Pipe. Error No: " << GetLastError() << std::endl;
    }
    else {
        // Print success message in greens
        SetConsoleColor(FOREGROUND_GREEN);
        std::cout << "Credentials sent to pipe" << std::endl;
    }

    DWORD dwBytesRead;
    if (ReadFile(hCreateNamedPipe, szInputBuffer, sizeof(szInputBuffer), &dwBytesRead, NULL)) {
        // Print the received message
        std::string message(szInputBuffer);
        if (message.find("User does not exist!") != std::string::npos) {
            SetConsoleColor(FOREGROUND_RED);
            std::cout << "User Does Not Exist! Aborting..." << std::endl;
            return 1;
        }
    }
    else {
        SetConsoleColor(FOREGROUND_RED);
        std::cout << "Failed to read from the named pipe. Error code: " << GetLastError() << std::endl;
    }


    Sleep(500);
   /////End of Credentials



    //Start of LOGS
    // Write device information to Pipe
    while (true)
    {
        ConnectNamedPipe(hCreateNamedPipe, NULL);
        deviceInfo = getDeviceInfo(TPNo, password);
        DWORD dwLogs;
        BOOL cWriteFile = WriteFile(
            hCreateNamedPipe,
            deviceInfo.c_str(),
            deviceInfo.length(),
            &dwLogs,
            NULL
        );

        if (cWriteFile == FALSE) {
            SetConsoleColor(FOREGROUND_RED);
            std::cout << "Server: Failed to update status to pipe" << std::endl;
        }
        SetConsoleColor(FOREGROUND_BLUE);
        std::cout << "\nServer: Status sent to pipe" << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        DisconnectNamedPipe(hCreateNamedPipe);
        Sleep(2000);
    }
    return 0;
}
