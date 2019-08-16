from modules import drone_server, drone_msp, drone_calculation
from threading import Thread, Lock
import time
if __name__ == '__main__':
    t = Thread(target=drone_server.run_server) #run drone server
    t.start()

    board = drone_msp.MultiWii("/dev/ttyACM0")
    try:
        while True:
            board.getData(drone_msp.MultiWii.ATTITUDE)
            #board.sendCMDreceiveATT(self, data_length, code, data)
            message = ("angx = {:+.2f} \t angy = {:+.2f} \t \
                        heading = {:+.2f} \t \elapsed = {:+.4f} \t"\
                        .format(float(board.attitude['angx']),float(board.attitude['angy']),\
                            float(board.attitude['heading']),float(board.attitude['elapsed'])))
            print(drone_server.command)
            print(drone_server.value)
            action=drone_calculation(drone_server.command, drone_server.value)
            print(action)
            print(message)
            time.sleep(0.1)
    except Exception. error:
        print ("Error on Main: "+str(error))