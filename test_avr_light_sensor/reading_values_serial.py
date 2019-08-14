# This code is to catch the data coming into the serial ports.

# also pay attention to the baud rate. If the baud rate is 9600 in arduino then it is 9600 here too.

import serial # import Serial Library that you need to import

# important. Check with arduino what is the port number and the baud rate
arduinoData = serial.Serial("com3", 9600) #Creating our serial object named arduinoData
arduinoData.flush() ## flushing the buffer 
j=0


while True: # While loop that loops forever
    while (arduinoData.inWaiting()==0): #Wait here until there is data' 
        pass #do nothing
    arduinoString = arduinoData.readline()  #read the line of text from the serial port
    print 'reading ', arduinoString
    #print 'float',float(arduinoString)
    j=j+1


