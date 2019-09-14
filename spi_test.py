import time
import spidev


def spi_carrier(command):
    result=[]
    for task in command:
        for letter in str(task).zfill(2):
            result.append(ord(letter))
    result.append(ord("\n"))
    return result

spi = spidev.SpiDev()
spi.open(0, 0)              # open(bus, device)

spi.mode = 0
spi.max_speed_hz = 1000000  # set transfer speed
while(1):#roll pitch yaw throttle
    #spi.xfer2([11])
   # spi.xfer2([ord('r')])
  #  spi.xfer2([22])
    
  #  spi.xfer2([ord('p')])
   # spi.xfer2([33])
    
   # spi.xfer2([ord('y')])
   # spi.xfer2([55])
    command=[32,32,32,0,0,0]
    result=spi_carrier(command)
    print(result)
    spi.xfer2(result)
    time.sleep(0.1)
spi.close()

