import socket
import time
from threading import Thread, Lock
command = 'S'
value = 0
lock=Lock()

def command_parser(msg):
    command_arr = msg.split(".", maxsplit=1)
    parsing_command = command_arr[0].upper()
    if (parsing_command=='S' or parsing_command=='A'): #case for stop or arming
        parsing_value = 0
    else:
        try:  #check value is integer
            parsing_value = int(command_arr[1])
        except:
            return 1 #error code 1 : value is not int

    if ((parsing_value<0) or (parsing_value>100)):
        return 2 #error code 2 : value is not between 0 and 100

    elif (parsing_command!='F' and parsing_command!='B' and
          parsing_command!='L' and parsing_command!='R' and
          parsing_command!='U' and parsing_command!='D' and
          parsing_command!='S' and parsing_command!='A'):
        return 3 #error code 3: command is not proper
    else: #acquire and change the value
        global command
        global value
        lock.acquire()
        try:
            command = parsing_command
            value = parsing_value
        finally:
            lock.release()
            return 0

def echo_handler(conn, addr, terminator="bye"):
    #reciving handler for each connection
    BUF_SIZE = 1024
    while True:
        data = conn.recv(BUF_SIZE)
        msg = data.decode()
        error_code = command_parser(msg)
        if error_code: #send error code if it exists
            print("error code : " + str(error_code))
            conn.sendall(("error code : " + str(error_code)).encode())
        if msg == terminator:
            conn.close()
            break

def run_server(host='', port=23452):
    with socket.socket() as sock:
        sock.bind((host, port))
        while True:
            sock.listen(3)
            conn, addr = sock.accept()
            # new thread process the new connection
            t = Thread(target=echo_handler, args=(conn, addr))
            t.start()
        sock.close()

if __name__ == '__main__':
    t = Thread(target=run_server)
    t.start()
    while True:
        print(command)
        print(value)
        time.sleep(1)
