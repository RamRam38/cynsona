#include <WiFi101.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID       "AirPennNet-Device"
#define WLAN_PASS       "penn1740wifi"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "mraheja"
#define AIO_KEY         "12ec2606c88d47ec8eb72750f74ed9a7"

int x = 0;
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/welcome-feed");

//DEFINES TIME
int starttime = 0;

//PORTS
const int LED = 1;
const int BUZZER = 5;
const int accX = 3;
const int accY = 2;
const int accZ = 1;
const int BUTTON = 2;

//INITAL VALUES OF ACCELEROMETER
int initX = 0, initY = 0, initZ = 0;

//TOLERANCE LEVEL FOR LOW AMOUNT OF MOVEMENT
int frame = 10;

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


void setup() {
  //Begin Serial on port 9600
  Serial.begin(9600);

  //Set up digital prots
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT);

  connectToWiFi();
  connectToAdafruit();

  //Blink the LED to show that the program is running
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);

}

void loop() {
  //Blinks the LED every 0.1 seconds if in initial response or emergency response state
  if (! mqtt.ping(3)) {
    // reconnect to adafruit io
    if (! mqtt.connected())
      connect();
  }
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
    }
  } else if (state == 1) {
    //Continually checks if the acceleration is further from initial than the "frame" threshold
    int curX = analogRead(accX);
    int curY = analogRead(accY);
    int curZ = analogRead(accZ);

    if (max(abs(curX - initX), max(abs(curY - initY), abs(curZ - initZ))) > frame) {
      //If it is, then initialize buzzer and blinking
      initialResponse();
      //Prints to serial monitor so that python can detect it
      char b[4];
      Serial.println("bad");
      String str = "1";
      for (int i = 0; i < str.length(); i++)
      {
        b[i] = str.charAt(i);
      }
      b[(str.length()) + 1] = 0;
      // Publish data
      if (!temperature.publish((char*)b)) {
        Serial.println("Sent");
      }
      //Moves to state 2
      state++;
      starttime = millis();
    }
  } else if (state == 2) {
    //Continually checks if the acceleration is further from initial than the "frame" threshold
    int curX = analogRead(accX);
    int curY = analogRead(accY);
    int curZ = analogRead(accZ);
    if (max(abs(curX - initX), max(abs(curY - initY), abs(curZ - initZ))) < frame && millis() - 3000 > starttime) {
      digitalWrite(LED, HIGH);
      analogWrite(BUZZER, 0);
      blinking = 0;
      state = 1;
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

void connectToWiFi()
{
  // Connect to WiFi access point.
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("WiFi connected!"));
}

void connectToAdafruit()
{
  connect();
}

void connect() {
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }
    if (ret >= 0)
      mqtt.disconnect();
    Serial.println(F("Retrying connection..."));
    delay(5000);
  }
  Serial.println(F("Adafruit IO Connected!"));
}

