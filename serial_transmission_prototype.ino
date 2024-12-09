#include <SoftwareSerial.h> 

SoftwareSerial XBee(0,1);
const int input = 0;
const int output = 1;
const int pps = 7;
const long interval = 1000;
const long interval1 = 2000;
unsigned long previousMillis = 0;

void setup() { 
  Serial.begin(9600);
  pinMode(input, INPUT);
  pinMode(output, OUTPUT);
  pinMode(pps, OUTPUT); 
  digitalWrite (pps, LOW);
}

void loop() { 
  unsigned long currentMillis = millis(); 
    if(Serial.available() > 13){
      for (int i = 0; i < 14; i++){
        Serial.print(Serial.read(), HEX);
        Serial.print(", ");
      }
      Serial.println();
    }
    if (currentMillis - previousMillis == interval) {
      digitalWrite(pps, HIGH);
    }
    if (currentMillis - previousMillis == interval1){
      previousMillis = currentMillis;
      digitalWrite(pps, LOW);
    }
    if (currentMillis == 10000){
      currentMillis == 0;
    }
}
