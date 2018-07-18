import serial

#Starts serial connection with arduino
ser = serial.Serial('COM11', 9600, timeout = 0.1)

#Records default message (so that we can ignore these later)
init = ser.readline();

while 1:
	try:
		#reads data from arduino
		x = ser.readline()
		#checks if the message is actually important
		if(x != init and x != None):
			#checks if arduino sent a message saying "bad"
			if("bad" in str(x)):
				#Prompts user to see if it was an accident
				read = input("Your computer might have been stolen! This wasn't an accident right? (Y/N) ")
				#Supposed to send message to arduino but doens't work
				if('N' in read):
					ser.write("1".encode());
	except:
		#stops the program if connection is lost
		break
