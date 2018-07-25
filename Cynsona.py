import serial
import time
from twilio import twiml
from twilio.rest import Client
from twilio.twiml.messaging_response import MessagingResponse
 
account_sid = 'AC2c14dd40e2a69216f19d9930ced46bc9'
auth_token = 'fb6090a6bfa61775432a89550ccf7609'
client = Client(account_sid, auth_token)
arduino = serial.Serial('COM11', 9600, timeout=.1)

from firebase import firebase

firebase = firebase.FirebaseApplication('https://ojo-mtsi.firebaseio.com/', None)

number = '+15108574274'
popo = '+19176547851'

def clear():
	x = firebase.get('/bad',None)
	if(x != None):
		for e in x:
			print(e)
			firebase.delete('/bad',e)
	
def check():
	x = firebase.get('/bad',None)
	if(x != None):
		for e in x:
			return x[e]
	return "bad"

def fullcheck():	
	clear()
	while 1:
		x = check()
		if(x != "bad"):
			return x
			break
def init():
	#Send text message to phone
	send(number,"Entering Initial State")
	
def contactPoPo():
	send(number,"What is your location")
	location = fullcheck()
	send(popo,"Someone has stolen a computer at " + location)
	
	
def bad():
	#Send Message asking wether it's actually bad
	send(number,"Someone stole your computer! Was this an actual theft?")
	temp = fullcheck()
	if("no" in temp.lower()):
		print("N")
		arduino.write('N'.encode())
	else:
		print("Y")
		arduino.write("Y".encode())
		send(number,"Contact the police?")
		temp = fullcheck()
		if("yes" in temp.lower()):
			contactPoPo()
		
def bad2():
	#Send Message asking wether it's actually bad
	send(number,"Someone disconnected your device! Contact the police?")
	temp = fullcheck()
	if("no" in temp.lower()):
		send(number,"Ok")
	else:
		contactPoPo()
		
		


	
	
def send(num, data):
	message = client.messages.create(
                              from_='+15103594713',
                              body=data,
                              to=num
                          )
	
started = False;
while True:
	try:
		s = arduino.readline()
		print(s)
		if("I" in str(s)):
			started = True
			print("Entering Initial State")
			init()
		if(started and "B" in str(s)):
			print("Bad State")
			bad()
	except:
		bad2()



	




		
