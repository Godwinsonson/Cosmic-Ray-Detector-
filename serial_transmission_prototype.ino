#include <SoftwareSerial.h> 

SoftwareSerial XBee(0,1);

const byte numChars = 14;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;

void setup() {
  Serial.begin(9600);
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(7, OUTPUT); 
}

void loop() {
  digitalWrite(7,LOW);
  recvWithStartEndMarkers();
  showNewData();
  delay (1000);
  digitalWrite(7,HIGH);
  recvWithStartEndMarkers();
  showNewData();
  delay (1000);
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '$';
    char endMarker = '*';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}