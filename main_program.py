from modules import drone_server, drone_msp, drone_calculation
from threading import Thread, Lock
import time
if __name__ == '__main__':
    t = Thread(target=drone_server.run_server) #run drone server
    t.start()

    board = drone_msp.MultiWii("/dev/ttyACM0")
    try:
        while True:
            action = [1500, 1500, 1500, 800, 1500, 1500, 1500, 1500] #default value
            #board.getData(drone_msp.MultiWii.ATTITUDE)
            board.sendCMDreceiveATT(16, drone_msp.MultiWii.SET_RAW_RC, action)
            '''message = ("angx = {:+.2f} \t angy = {:+.2f} \t \
                        heading = {:+.2f} \t \elapsed = {:+.4f} \t"\
                        .format(float(board.attitude['angx']),float(board.attitude['angy']),\
                            float(board.attitude['heading']),float(board.attitude['elapsed'])))'''
            print(drone_server.command)
            print(drone_server.value)
            if (drone_server.command=='A'):
                board.arm()
            elif (drone_server.command=='U'):
                
            elif (drone_server.command=='D'):
            else:
                action=drone_calculation(drone_server.command, drone_server.value)
            print(action)
            #print(message) #for debug
            time.sleep(0.1)
    except Exception. error:
        print ("Error on Main: "+str(error))