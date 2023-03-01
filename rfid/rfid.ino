// Based on https://www.instructables.com/Reading-RFID-Tags-with-an-Arduino/
// Expanded by Michael R. Gustafson II to store code

char val = 0; // variable to store the data from the serial port
int len = 12;


#include <SoftwareSerial.h>

#define TxPin 14

SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

void setup() {
  Serial.begin(9600); // connect to the serial port for the monitor
  Serial1.begin(9600); // connect to the serial port for the RFID reader
  mySerial.begin(9600); //lcd serial
  mySerial.write(17); // backlight
  delay(10);
}

void loop () {
  char rfidData[len+1] = {};
  int get_more = 1;
  int i = 0;
  
  while(get_more == 1){
    if(Serial1.available() > 0) {
    val = Serial1.read();
  
      // Handle unprintable characters
      switch(val) {
        case 0x2: break;               // start of transmission - do not save
        case 0x3: get_more = 0; break; // end of transmission - done with code
        case 0xA: break;               // line feed - do not save
        case 0xD: break;               // carriage return - do not save
        default:  rfidData[i]=val; i+=1;  break; // actual character
      }
    }
  }
  Serial.println(rfidData);
  mySerial.println(rfidData);
}