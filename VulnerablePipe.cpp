#include <iostream>
#include <Windows.h>


int main() {



    std::string TPNo;
    std::string password;
    std::cout << "              _                      _     _            __             _            ___                 " << std::endl;
    std::cout << " /\\   /\\_   _| |_ __   ___ _ __ __ _| |__ | | ___      / /  ___   __ _(_)_ __      / _ \\__ _  __ _  ___ " << std::endl;
    std::cout << " \\ \\ / | | | | | '_ \\ / _ | '__/ _` | '_ \\| |/ _ \\    / /  / _ \\ / _` | | '_ \\    / /_)/ _` |/ _` |/ _ \\ " << std::endl;
    std::cout << "  \\ V /| |_| | | | | |  __| | | (_| | |_) | |  __/   / /__| (_) | (_| | | | | |  / ___| (_| | (_| |  __/ " << std::endl;
    std::cout << "   \\_/  \\__,_|_|_| |_|\\___|_|  \\__,_|_.__/|_|\\___|   \\____/\\___/ \\__, |_|_| |_|  \\/    \\__,_|\\__, |\\___| " << std::endl;
    std::cout << "                                                                 |___/                       |___/      " << std::endl;
    std::cout << "\n\n\n\n\nEnter your TP Number: ";
    std::cin >> TPNo;
    std::cout << "Enter your password: ";
    std::cin >> password;

    //Named Pipe Local Variable
    HANDLE hCreateNamedPipe;
    char szInputBuffer[1023];
    char szOutputBuffer[1023];
    DWORD dwszInputBuffer = sizeof(szInputBuffer);
    DWORD dwszOutputBuffer = sizeof(szOutputBuffer);


    //ConnectNamedPipe Variable
    BOOL bConnectNamedPipe;


    //WriteFile Local Variable
    BOOL bWriteFile;
    char szWriteFileBuffer[1023] = "Hello from main Process!";
    DWORD dwWriteBufferSize = sizeof(szWriteFileBuffer);
    DWORD dwNoBytesWrite;


    //Flush file buffer


    //CreateNamedPipe
    hCreateNamedPipe = CreateNamedPipe(
        "\\\\.\\pipe\\vulnerablepipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        dwszOutputBuffer,
        dwszInputBuffer,
        0,
        NULL
    );

    if (hCreateNamedPipe == INVALID_HANDLE_VALUE) {

        std::cout << "NamedPipe creation failed. Error code: " << GetLastError() << std::endl;
    }
    std::cout << "Pipe creation successful" << std::endl;
    


    //Connect to pipe
    std::cout << "Connecting to pipe..." << std::endl;
    bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);
    std::cout << "Succesfully connected to Pipe" << std::endl;

    bWriteFile = WriteFile(

        hCreateNamedPipe,
        szWriteFileBuffer,
        dwWriteBufferSize,
        &dwNoBytesWrite,
        NULL
    );


    if (bWriteFile == FALSE) {

        std::cout << "Failed to write into Pipe. Error No: " << GetLastError() << std::endl;
    }
    std::cout << "Successfully written to file through pipe" << std::endl;

    return 0;
}
