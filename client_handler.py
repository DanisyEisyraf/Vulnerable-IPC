import win32pipe
import win32file
import os
import pyodbc
import time
import pywintypes

PIPE_NAME = r'\\.\pipe\hackthispipe'
SERVER = '0X72696B654C5F4\\USETHISSERVER'
DATABASE = 'ASC_Database'
TABLE = 'Users'                             

def main():
    time.sleep(1.5)

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
        auth_flag = ""

        #Retrieve credentials from the pipe
        credentials = win32file.ReadFile(pipe_handle, 4096)[1].decode()

        #Separate the ID and password and put in different variables
        ID_no, password = credentials.split('|')
        if check_credentials(ID_no, password):
            auth_flag = "Authenticated"
            print("Login successful")
            win32file.WriteFile(pipe_handle,auth_flag.encode('utf-8'))
            log_monitoring()
            
        else:
            auth_flag = "User does not exist!"
            win32file.WriteFile(pipe_handle,auth_flag.encode('utf-8'))
            print("Login failed")

    except Exception as e:
        print("Error:", e)


def check_credentials(ID, password):
    # Connect to SQL Server using Windows authentication
    conn = pyodbc.connect('DRIVER={SQL Server};SERVER='+SERVER+';DATABASE='+DATABASE+';Trusted_Connection=yes;')
    cursor = conn.cursor()

    # Construct SQL query with potential SQL injection vulnerability
    query = f"SELECT COUNT(*) FROM {TABLE} WHERE ID_No='{ID}' AND password='{password}'"
    
    # Print the SQL query for debugging purposes
    print("SQL Query:", query)

    # Execute SQL query
    cursor.execute(query)
    row = cursor.fetchone()

    # Close connection
    conn.close()

    # Return True if credentials are valid, False otherwise
    return row[0] == 1 if row else False
def log_monitoring():
    while True:
        try:
            pipe = win32file.CreateFile(
                PIPE_NAME,
                win32file.GENERIC_READ,
                0,
                None,
                win32file.OPEN_EXISTING,
                0,
                None) 
            status  = win32file.ReadFile(pipe, 4096)[1].decode()
            tp_no, device_name, processID, Time, state = status.split('|')
            print("\n")
            
            print("\x1b[0m#################################################")
            # ANSI escape code for green color
            GREEN = "\x1b[32m"
            # ANSI escape code to reset color
            RESET = "\x1b[0m"

            # Print each line in green
            print(GREEN + "Response from Client Handler: " + RESET)
            print(GREEN + "TP No:" + RESET, tp_no)
            print(GREEN + "Device Name:" + RESET, device_name)
            print(GREEN + "PID:" + RESET, processID)
            print(GREEN + "Time:" + RESET, Time)
            print(GREEN + "Status:" + RESET, state)
            print("#################################################")
            time.sleep(2)  # Wait for the next message
            win32file.CloseHandle(pipe)
        except pywintypes.error as e:
            if e.args[0] == 231:  # ERROR_PIPE_BUSY
                print("\n\n\x1b[33mClient: Pipe is busy, retrying...")
                time.sleep(1)  # Wait before retrying
                continue
            elif e.args[0] == 2:
                print("Client: Server is not running")
                break
            else:
                print("Client: Error:", e)
                break

if __name__ == "__main__":
    main()