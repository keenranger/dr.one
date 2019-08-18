from modules import drone_server, drone_msp, drone_calculation
from threading import Thread, Lock
import time
import smbus

if __name__ == '__main__':
#    arduino_addr = 0x04
 #   I2Cbus = smbus.SMBus(1) 
    
    t = Thread(target=drone_server.run_server) #run drone server
    t.start()
    board = drone_msp.MultiWii("/dev/ttyACM0") #connect via usb
    #board = drone_msp.MultiWii("/dev/ttyAMA0") #connect via tx/rx
    try:
        while True:
            action = [1500, 1500, 1500, 800] #default value
            
            board.getData(drone_msp.MultiWii.ATTITUDE)
            board.sendCMDreceiveATT(8, drone_msp.MultiWii.SET_RAW_RC, action)
            message = ("angx = {:+.2f} \t angy = {:+.2f} \t heading = {:+.2f} \t \elapsed = {:+.4f} \t"\
                        .format(float(board.attitude['angx']),float(board.attitude['angy']),\
                            float(board.attitude['heading']),float(board.attitude['elapsed'])))
            print(drone_server.command)
            if (drone_server.command=='A'):
                board.arm()
                print('arm done')
                drone_server.command='S'
            elif (drone_server.command=='U'):
                pass
            elif (drone_server.command=='D'):
                pass
            else:
                action=drone_calculation.command_to_action(drone_server.command, drone_server.value)
                pass   
            print(action)
            print(message) #for debug
            time.sleep(0.1)
    except Exception as error:
        print ("Error on Main: "+str(error))
