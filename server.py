from flask import Flask, request
from twilio import twiml
from twilio.twiml.messaging_response import MessagingResponse
from firebase import firebase

firebase = firebase.FirebaseApplication('https://ojo-mtsi.firebaseio.com/', None)
 
app = Flask(__name__)
 

 
@app.route('/sms', methods=['GET','POST'])
def sms():
	number = request.form['From']
	message_body = request.form['Body']
 
	result = firebase.post('/bad', message_body)
	print(result)
	
	resp = MessagingResponse()
	resp.message('Hello {}, you said: {}'.format(number, message_body))
	return str(resp)
 
if __name__ == '__main__':
	app.run(debug=True)