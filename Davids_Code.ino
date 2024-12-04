#include <SoftwareSerial.h>
#include <Adafruit_GPS.h> // Gps Head file


SoftwareSerial XBee(10, 11); // RX, TX //starts xbee serial
Adafruit_GPS GPS(&XBee); //does not work but intended to make xbee like the serial port 

#define pps 3
#define space 100 
#define muons A0 // reades the voltage of the saved signal daqboard
#define returns 14
#define trigger 2


const byte numChars = 130;  //maximum size of NMEA sentence 
boolean newData = false;    
char receivedChars[numChars];  
char tempChars[numChars];        // temporary array for use by strtok() function
char GGA[numChars];              // Stores NMEA sentence 
unsigned long int GSA[numChars]; //Stores raw clock cycle difference
long int GGG[numChars];          // Stores converted clock cycles
char r;

unsigned long int overflows = 0, o1 = 0, o2 = 0; //how many times the timer1 overflows
unsigned int temp = 0,temp1=0; // it's a count for TIMER1
float clockFreq=16.003312;
int p = 0; 
int muonsvoltage[numChars];

void setup() {
    Serial.begin(9600);
    XBee.begin(9600);
    GPS.begin(9600);
    TIMSK1 = _BV(TOIE1);
    TCCR1A = 0;  
    TCCR1B = 0;  
    TCCR1C = 0;  
    TCNT1 = 0;
    pinMode(trigger,INPUT); //Sets pinmodes 
    pinMode(pps,INPUT);
    pinMode(muons,INPUT);
    pinMode(returns,OUTPUT);
    Serial.print("Starting"); 
    Serial.print('\n');
    analogReference(INTERNAL2V56);
    set16(); //sets bitshift scale
    delay(1000);
    /*/delay(10000);  If a solution to gps communication is found ensure there is a sufficient delay to send commands
    
    GPS.sendCommand("$PGCMD,33,0*6D");
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    
    */
    
    Serial.print("Ready");
    Serial.print('\n');
    Serial.print("Time");
    Serial.print("\t");
    Serial.print("\t");
    Serial.print("Milis");
    Serial.print("\t");
    Serial.print("\t");
    Serial.print("Voltage");
    Serial.print('\n');
    attachInterrupt(digitalPinToInterrupt(pps),ppS,RISING);  //INTERRUPTS FOR TRIGGER AND 1PPS
    attachInterrupt(digitalPinToInterrupt(trigger),Trigger,RISING);
}

void loop() {
    
    recvWithStartEndMarkers();
    
    if (newData == true) { 
        
        strcpy(tempChars, receivedChars); // this temporary copy is necessary to protect the original data because strtok() replaces the commas with \0
        parseData();
        //Trigger();
        newData = false;
        
      }
  
   
}

//============

void recvWithStartEndMarkers() { // Reads in data and seperates into sentence, the data may look corrupted if you try to print these arrays out!

    static boolean recvInProgress = false; //tells loop to start 
    static byte ndx = 0; // index you are using to iterate 
    char startMarker = '$'; // Set this to begining of the sentence
    char endMarker = '*'; // Set this to end of sentence, easiest is * (they aren't consistent) 
    char rc;  // Char used to store incoming Serial data 

    if (XBee.available() > 0 && newData == false) {              //waiting for XBee to be ready to give data, checks if loop is finished
        rc = XBee.read(); 
       
        if (recvInProgress == true) { 
            if (rc != endMarker) {                                 //Searches for end marker set above , * 
                receivedChars[ndx] = rc;                          // Iterates each index 
                ndx++; 
                if (ndx >= numChars) {                          //Checks if index has reached maximum (numChar) 
                     ndx = numChars - 1;                       // Stops short to ensure overflow does not happen
                }
            }
            else {
                receivedChars[ndx] = '\0';               // terminate the string to avoid memory leaks! (always do this)
                //Serial.print(receivedChars);
               // Serial.print('\n');
               // Serial.print('\n');
                recvInProgress = false;            // restarts loop 
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() { // Takes read-in data and parses 

      
    char * strtokIndx; //pointer for STRTOK and STRCMP 
   
     
    
    strtokIndx = strtok(tempChars,","); // get the first part - the string 
        
     if (strcmp(strtokIndx, "GPRMC") == 0){  //compares first part of string to whatever NMEA sentence you want
                  strcpy(GGA, receivedChars); //copies to string GG 
                  Serial.print(GGA);
                  strcpy(GGA, '\0'); // terminates string to avoid memory leaks
                 // Serial.print(GGA);
                  Serial.print('\n');
                  for (int i = 6; i<12;i++){
                        r = (GGA[i]); //6-15 for timing info, pick the piece of data you wish to read out from the NMEA        
                        Serial.write((r)); 
                        
       
    }   
       //Serial.print('\n');
        }
    
      
}
//============




void ppS(){ //1PPS read in 
  
  
  TCCR1B=1; //Starts counter (this value is set to 1 for ON) 
  temp = TCNT1; // Stores current clock cycle 
  o1 = overflows; // counts overflows internally 
  
  if(o1<=100) return; //resets counter to avoid overflows 
     TCNT1=0;
     overflows = 0;

  clockFreq = float((((o1<<16)+temp))+39)/1000000.; // Calculates time between two PPS 
} 




//==============


void Trigger(){   //Trigger to read analog voltage and output gps data 
    
    temp = TCNT1; // Sets temp to clock cycle at which trigger happens 
    o2=overflows; // counts overflows internally, different ones needed to keep track inside loops properly 
    
    
    muonsvoltage[p] = analogRead(muons); // reads in voltages 
    
   // digitalWrite(returns,HIGH);   // return signal
 //   digitalWrite(returns,LOW);
    
    GGG[p] = ((o2<<16)+temp);    // << means shifting o2 by 16 bits, this should be the same as taking the number of overflows (o2) and multiplying by total # of clock cycles, 256
    GSA[p] = GGG[p]/(clockFreq); /// clock cycles between 1PPS and NMEA setence
    
     
     /// Gps Data output 
     
     Serial.print('\t');
     Serial.print('\t');
   
     Serial.print(GSA[p]);
     Serial.print('\t');
     Serial.print('\t');
     Serial.print(float(muonsvoltage[p])*2.56/1024);
     Serial.print('\t');
    // Serial.print(p);
     Serial.print('\n');
     Serial.print('\n');
     p++; 
     test();
     //*/
}

//=================================================


 ISR(TIMER1_OVF_vect) //Counts number of overflows during a second  
{  
  overflows++;
  
}

void set16() // Sets board settings for overclock 
{
  ADCSRA |= (0<<ADPS2);
  ADCSRA &= ~(0<<ADPS1);
  ADCSRA &= ~(0<<ADPS0);
 }
 void test(){    // Tests if the index is beyond the maximum value to prevent overflows in output
  if(p==numChars) p=0;
  
  } 
