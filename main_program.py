from modules import drone_server, drone_msp, drone_calculation
from threading import Thread, Lock
import time
import spidev

if __name__ == '__main__':
    spi = spidev.SpiDev()
    spi.open(0, 0)              # open(bus, device)
    spi.mode = 0
    spi.max_speed_hz = 1000000  # set transfer speed

    pid = drone_calculation.PID()

    t = Thread(target=drone_server.run_server) #run drone server
    t.start()

    board = drone_msp.MultiWii("/dev/ttyACM0") #connect via usb
    try:
        while True:
            ppm_command = [1500, 1500, 1500, 1000] #default value 
            bldc=[0,0] # 0~1000

            board.getData(drone_msp.MultiWii.ATTITUDE)
            attitude=[float(board.attitude['angx']), float(board.attitude['angy']), float(board.attitude['heading'])]
            print(attitude)
            bldc=pid.calculate_bldc(ppm_command[2],ppm_command[3], attitude[2]) #yaw, throttle, heading
            
            print(drone_server.command)
            ppm_command=drone_calculation.command_to_ppm_command(drone_server.command, drone_server.value)

            spi.xfer2(drone_calculation.spi_carrier(ppm_command+bldc))# command를 spi carrier에 담아 전송

            
            print('check2')
            time.sleep(0.01)
    except Exception as error:
        print ("Error on Main: "+str(error))
        spi.close()
