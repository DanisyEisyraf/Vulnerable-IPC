import win32pipe
import win32file
import os
import pyodbc
import time

PIPE_NAME = r'\\.\pipe\hackthispipe'
SERVER = '0X72696B654C5F4\\USETHISSERVER'
DATABASE = 'ASC_Database'
TABLE = 'Users'

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

        credentials = win32file.ReadFile(pipe_handle, 4096)[1].decode()
        ID_no, password = credentials.split('|')
        #print(credentials)
        if check_credentials(ID_no, password):
            print("Login successful")
            while True:
                log_monitoring(pipe_handle)
                time.sleep(2)
        else:
            print("Login failed")

    except Exception as e:
        print("Error:", e)



def check_credentials(ID, password):
    # Connect to SQL Server using Windows authentication
    conn = pyodbc.connect('DRIVER={SQL Server};SERVER='+SERVER+';DATABASE='+DATABASE+';Trusted_Connection=yes;')
    cursor = conn.cursor()

    # Execute SQL query to check credentials
    query = f"SELECT COUNT(*) FROM {TABLE} WHERE ID_No=? AND password=?"
    cursor.execute(query, (ID, password))
    row = cursor.fetchone()
    
    # Close connection
    conn.close()

    # Return True if credentials are valid, False otherwise
    return row[0] == 1 if row else False

def log_monitoring(pipe_handle):

        status = win32file.ReadFile(pipe_handle, 4096)[1].decode()
        tp_no, device_name, processID, Time, state = status.split('|')

        print("\n\n")
        print("TP No:", tp_no)
        print("Device Name:", device_name)
        print("PID:", processID)
        print("Time:", Time)
        print("Status:", state)
        win32file.CloseHandle(pipe_handle)
        


if __name__ == "__main__":
    main()
