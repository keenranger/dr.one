import socket
HOST = '192.168.0.34' 
# Server IP or Hostname
PORT = 23452 
# Pick an open Port (1000+ recommended), must match the client sport

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print('Connected by', addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print('1')
            conn.sendall(data)
            print(data)