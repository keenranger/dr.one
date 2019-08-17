# hello
def command_to_action(command, value):
    action = [1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500]
    if (command == 'S'): #stop = hovering
    elif (command == 'F'): #Forward
        action[1] += float(value*5)
    elif (command == 'B'): #Backward
        action[1] -= float(value*5)
    elif (command == 'L'): #Left
        action[0] -= float(value*5)
    elif (command == 'R'): #Right
        action[0] += float(value*5)
    #elif (command == 'A'): #Arming
    #    action['yaw'] = 2000
    #    action['throttle'] = 1000
    return action
