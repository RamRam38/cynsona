int x = 0;


//DEFINES TIMint starttime = 0;

//PORTS
const int LED = 1;
const int lamp_pin = 1;
const int BUZZER = 5;
const int accX = 3;
const int accY = 2;
const int accZ = 1;
const int BUTTON = 2;

//INITAL VALUES OF ACCELEROMETER
int initX = 0, initY = 0, initZ = 0;

//TOLERANCE LEVEL FOR LOW AMOUNT OF MOVEMENT
int frame = 100;

/*
   state = 0 IDLE
   state = 1 PROTECTION MODE
   state = 2 INITIAL RESPONSE
   state = 3 EMERGENCY RESPONSE
*/
int state = 0;

/*
   Variables for blinking the LED
*/
int blinking = 0, ledState = 1, prev = 0, t = 100;

int starttime = 0;

void setup() {
  //Begin Serial on port 9600
  Serial.begin(9600);

  //Set up digital prots
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT);

  //Blink the LED to show that the program is running
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);

  delay(1000);
  Serial.println("I");

}

void loop() {
  
  //Blinks the LED every 0.1 seconds if in initial response or emergency response state
  
  if (blinking) {
    if (millis() > prev + t) {
      prev = millis();
      ledState ^= 1;
    }

    digitalWrite(LED, ledState);
  }

  //In IDLE state, keeps checking if button is pressed
  if (state == 0) {
    //Makes state into 1 if BUTTON is pressed
    state ^= digitalRead(BUTTON);

    if (state == 1) {
      //When switching between states, measure the initial acceleration
      start();
      initX = analogRead(accX);
      initY = analogRead(accY);
      initZ = analogRead(accZ);
      Serial.println("I");
    }
    
  } else if (state == 1) {
    //Continually checks if the acceleration is further from initial than the "frame" threshold
    int curX = analogRead(accX);
    int curY = analogRead(accY);
    int curZ = analogRead(accZ);

    if (max(abs(curX - initX), max(abs(curY - initY), abs(curZ - initZ))) > frame) {
      //If it is, then initialize buzzer and blinking
      
      initialResponse();
      Serial.println("B");
      
      //Moves to state 2
      state++;

      starttime = millis();
    }
  } else if (state == 2) {

    if(Serial.available() > 0){
      char r = Serial.read();
      if(r == 'Y'){
        analogWrite(BUZZER,100);
      }else if(r == 'N'){
        state = 1;
        analogWrite(BUZZER,0);
        blinking = 0;
        digitalWrite(LED,1);
      }
    }
  }

}

void start() {
  //Starts LED when button is pressed
  digitalWrite(LED, HIGH);
}

void initialResponse() {
  //Starts initial response with buzzer and blinking
  analogWrite(BUZZER, 50);
  blinking = 1;
}

