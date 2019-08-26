from modules import drone_server, drone_msp, drone_calculation
from threading import Thread, Lock
import time
import smbus

if __name__ == '__main__':
    arduino_addr = 0x04
    I2Cbus = smbus.SMBus(1)
    pid = drone_calculation.PID()

    t = Thread(target=drone_server.run_server) #run drone server
    t.start()

    board = drone_msp.MultiWii("/dev/ttyACM0") #connect via usb
    try:
        while True:
            ppm_command = [1500, 1500, 1500, 1000] #default value
            bldc=[0,0]
            board.getData(drone_msp.MultiWii.ATTITUDE)
            attitude=[float(board.attitude['angx']), float(board.attitude['angy']), float(board.attitude['heading'])]
            print(attitude)
            '''message = ("angx = {:+.2f} \t angy = {:+.2f} \t heading = {:+.2f} \t \elapsed = {:+.4f} \t"\
                        .format(float(board.attitude['angx']),float(board.attitude['angy']),\
                            float(board.attitude['heading']),float(board.attitude['elapsed'])))'''
            print(drone_server.command)
            ppm_command=drone_calculation.command_to_ppm_command(drone_server.command, drone_server.value)

            if (drone_server.command=='A'):
                print('arm done')
                drone_server.command='S'
            
            bldc=pid.calculate_bldc(ppm_command[2],ppm_command[3], attitude[2]) #yaw, throttle, heading
            for ppm_block in ppm_command:
                block=str(ppm_block)
                I2Cbus.write_i2c_block_data(arduino_addr, 47, [int(block[0]), int(block[1]), int(block[2]), int(block[3])])
            for bldc_block in bldc:
                block=str(bldc_block+1000)#for box matching +1000
                I2Cbus.write_i2c_block_data(arduino_addr, 47, [int(block[0]), int(block[1]), int(block[2]), int(block[3])]) 
            I2Cbus.write_byte_data(arduino_addr, 47)
            time.sleep(0.3)
    except Exception as error:
        print ("Error on Main: "+str(error))
