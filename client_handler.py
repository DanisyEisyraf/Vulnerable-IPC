import win32pipe
import win32file
import os

PIPE_NAME = r'\\.\pipe\hackthispipe'

def main():
    try:
        # Connect to the named pipe
        pipe_handle = win32file.CreateFile(
            PIPE_NAME,
            win32file.GENERIC_READ,
            0,
            None,
            win32file.OPEN_EXISTING,
            0,
            None
        )
        print("Connected to named pipe")

        # Read data from the pipe
        data = win32file.ReadFile(pipe_handle, 4096)[1].decode()
        
        # Deserialize data to extract TP number and password
        tp_no, password = data.split('|')

        print("Received TP Number:", tp_no)
        print("Received Password:", password)

        # Close the pipe handle
        win32file.CloseHandle(pipe_handle)

    except Exception as e:
        print("Error:", e)

if __name__ == "__main__":
    main()
