def spi_carrier(command):
    result=[]
    for task in command:
        for letter in str(task).zfill(4):
            result.append(ord(letter))
    result.append(ord("\n"))
    return result

def command_to_ppm_command(command, value):
    ppm_command = [1500, 1500, 1500, 1500] #roll pitch yaw throttle
    if (command == 'S'): #stop = hovering
        pass
    elif (command == 'F'): #Forward
        ppm_command[1] += int(value * 5)
    elif (command == 'B'): #Backward
        ppm_command[1] -= int(value * 5)

    elif (command == 'L'): #Left
        ppm_command[0] -= int(value * 5)
    elif (command == 'R'): #Right
        ppm_command[0] += int(value * 5)

    elif (command == 'U'): #Up
        ppm_command[3] += int(value * 5)
    elif (command == 'D'): #Down
        ppm_command[3] -= int(value * 5)

    elif (command == 'A'): #Arming
        ppm_command[2] = 2000 #yaw
        ppm_command[3] = 1000 #throttle
    return ppm_command

class PID:
    def __init__(self):
        self.diffAngle=0
        self.angle=0
        self.preAngle=0
        self.control=0
        self.kp=1
        self.ki=0.1
        self.kd=0
        self.p=0
        self.i=0
        self.d=0
        self.err=0
        self.err_prev=0
        self.de=0
        self.dt=0
        self.final=0
    def calculate_bldc(self, yaw, throttle, heading):
        throttle-=1000
        self.control = (yaw-1500)*45/500 +10
        self.angle=heading
        self.diffAngle=self.angle-self.preAngle
        self.preAngle=self.angle

        self.err=self.control-self.diffAngle
        self.dt=0.001
        self.p = self.err * self.kp
        self.i = self.i + self.err * self.dt * self.ki
        self.i = constrain(self.i, -10, 10)
        self.final = self.p + self.i
        self.final = constrain(self.final, -10, 10)
        if (self.final>0):
            bl0_setValue = -abs(self.final) / 2.
            bl1_setValue = +abs(self.final) / 2.
        else:
            bl0_setValue = +abs(self.final) / 2.
            bl1_setValue = -abs(self.final) / 2.
        bl0_setValue +=throttle
        bl1_setValue +=throttle
        bl0_setValue = constrain(bl0_setValue, 0, 1000)
        bl1_setValue = constrain(bl1_setValue, 0, 1000)
        bl0=int(bl0_setValue)
        bl1=int(bl1_setValue)
        return [bl0, bl1]
        
def constrain(val, min_val, max_val):
    return min(max_val, max(min_val, val))
