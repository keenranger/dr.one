# hello
def command_to_ppm_command(command, value):
    ppm_command = [1500, 1500, 1500, 1500]
    if (command == 'S'): #stop = hovering
        pass
    elif (command == 'F'): #Forward
        action[1] += int(value*5)
    elif (command == 'B'): #Backward
        action[1] -= int(value*5)

    elif (command == 'L'): #Left
        action[0] -= int(value*5)
    elif (command == 'R'): #Right
        action[0] += int(value*5)  

    elif (command == 'U'): #Up
        action[3] -= int(value*5)
    elif (command == 'D'): #Down
        action[3] += int(value*5)
        
    elif (command == 'A'): #Arming
        action[2] = 2000 #yaw
        action[3] = 1000 #throttle
    return ppm_command
