#include <iostream>
#include <Windows.h>

void SetConsoleColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
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

    // Reset color to bright white
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    std::cout << "Enter your TP Number: ";
    std::cin >> TPNo;
    std::cout << "Enter your password: ";
    std::cin >> password;

    std::string dataToSend = TPNo + "|" + password;

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

    // Connect to pipe
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
        // Print success message in green
        SetConsoleColor(FOREGROUND_GREEN);
        std::cout << "WriteFile Successful" << std::endl;
    }




    return 0;
}
