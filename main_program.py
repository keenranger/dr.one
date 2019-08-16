from modules import drone_server
from threading import Thread, Lock
import time
if __name__ == '__main__':
    t = Thread(target=drone_server.run_server)
    t.start()
    while True:
            print(drone_server.command)
            print(drone_server.value)
            time.sleep(1)
