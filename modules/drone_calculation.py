# hello
def command_to_action(command, value):
    action = {'roll' = 1500, 'pitch' = 1500 'yaw' = 1500, 'throttle' = 1500}
    if (command == 'S'): #stop = hovering
    elif (command == 'F'): #Forward
        action['pitch'] += float(value*5)
    elif (command == 'B'): #Backward
        action['pitch'] -= float(value*5)
    elif (command == 'L'): #Left
        action['roll'] -= float(value*5)
    elif (command == 'R'): #Right
        action['roll'] += float(value*5)
    elif (command == 'A'): #Arming
        action['yaw'] = 2000
        action['throttle'] = 1000
    return action
