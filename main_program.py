from modules import drone_server, drone_msp, drone_calculation
from threading import Thread, Lock
import time
import smbus

if __name__ == '__main__':
    imu_addr = 0x68

    I2Cbus = smbus.SMBus(1)

    t = Thread(target=drone_server.run_server) #run drone server
    t.start()
    board = drone_msp.MultiWii("/dev/ttyACM0") #connect via usb
    try:
        while True:
            action = [1500, 1500, 1500, 800] #default value
            board.getData(drone_msp.MultiWii.ATTITUDE)
            message = ("angx = {:+.2f} \t angy = {:+.2f} \t heading = {:+.2f} \t \elapsed = {:+.4f} \t"\
                        .format(float(board.attitude['angx']),float(board.attitude['angy']),\
                            float(board.attitude['heading']),float(board.attitude['elapsed'])))
            print(drone_server.command)
            if (drone_server.command=='A'):
                print('arm done')
                drone_server.command='S'
            elif (drone_server.command=='U'):
                pass
            elif (drone_server.command=='D'):
                pass
            else:
                pass   
            print(action)
            print(message) #for debug
            time.sleep(0.1)
    except Exception as error:
        print ("Error on Main: "+str(error))
