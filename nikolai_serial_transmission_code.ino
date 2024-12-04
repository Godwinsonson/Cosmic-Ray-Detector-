#include <SoftwareSerial.h> 

SoftwareSerial XBee(0,1);

void setup() { 
  Serial.begin(9600);
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(7, OUTPUT); 
}

void loop() { 
  digitalWrite(7, HIGH);
  if(Serial.available() > 13){
    for (int i = 0; i < 14; i++){
      Serial.print(Serial.read(), HEX);
      Serial.print(", ");
    }
    Serial.println();
  }
  delay(1000);
  digitalWrite(7, LOW);
    if(Serial.available() > 13){
      for (int i = 0; i < 14; i++){
        Serial.print(Serial.read(), HEX);
        Serial.print(", ");
      }
      Serial.println();
    }
  delay (1000);
}