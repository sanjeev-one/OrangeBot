//Pins for QTI connections on board
#define lineSensor1 47 // left
#define lineSensor2 51  // middle 
#define lineSensor3 52 // right  

int hash = 1; // Hashmark number
int finalCode = 0; // Final code

// Define pins for built-in RGB LED
#define redpin 45
#define greenpin 46
#define bluepin 44


#include <Servo.h>                           // Include servo library
 
Servo servoLeft;                             // Declare left and right servos
Servo servoRight;







void setup() {
Serial.begin(9600); //start the serial monitor so we can view the output
servoLeft.attach(11);                      // Attach left signal to pin 13
servoRight.attach(12);                     // Attach right signal to pin 12
 maneuver(0, 0, 20); 

//RFID code & Serial:

Serial1.begin(9600); // connect to the serial port for the RFID reader
  //mySerial.begin(9600); //lcd serial
  //mySerial.write(17); // backlight
//XBEE serial start
Serial2.begin(9600);



}
void loop() {


  float qti1 = rcTime(lineSensor1); // left signal
  float qti2 = rcTime(lineSensor2); // middle signal
  float qti3 = rcTime(lineSensor3); // right signal

float ndShade;                             // Normalized differential shade
  ndShade = qti3 / (qti1+qti3) - 0.5;   // Calculate it and subtract 0.5
//Serial.println(ndShade); // Print


  int speedLeft, speedRight;                 // Declare speed variables





	// slow down inner wheel:

	if (ndShade > 0.0)                         // Shade on right?
  {                                          // Slow down left wheel
    speedRight = int(200.0 - (ndShade * 1000.0));
    speedRight = constrain(speedRight, -200, 200);
    speedLeft = 200;                        // Full speed right wheel
  }
  else                                       // Shade on Left?
  {                                          // Slow down right wheel
    speedLeft = int(200.0 + (ndShade * 1000.0));
    speedLeft = constrain(speedLeft, -200, 200);
    speedRight = 200;                         // Full speed left wheel
  }   
	 



maneuver(speedLeft, speedRight, 20);       // Set wheel speeds


 // delay(200);
 
 //Serial.print("left ");
 //Serial.println(qti1);
 //Serial.print("middle ");
// Serial.println(qti2);
 //Serial.print("right ");
 //Serial.println(qti3);
 
// 1 is light




 int state = 4*(qti1 <500 ) + 2*(qti2<500 ) + (qti3 < 500);

//Serial.println(state);


switch (state) {


case 0: // hashmark
  speedLeft = 0;
  speedRight = 0;
	maneuver(speedLeft, speedRight, 20);       // Set wheel speeds

	if (hash == 1){
	//red color
	set_RGBi(255, 0, 0);
  RFID();

	}
	if (hash == 2){
	//yellow color
	set_RGBi(255, 255, 0);
  RFID();

	}
	if (hash == 3){
	//green color
	set_RGBi(0, 255, 0);
  RFID();

	}
	if (hash == 4){
	//blue color
	set_RGBi(0, 0, 255);
  RFID();

	}
	if (hash == 5){
	//purple color
	set_RGBi(125, 0, 225);
  RFID();
	
	}
	if (hash == 6){
	hash = 0;
	set_RGBi(0, 0, 0);
  RFID();
  Serial.println("Final Code: ");
  Serial.println(finalCode);
	while(1==1){
		maneuver(0,0,20); //freeze bot 
	}
	}

  delay(1000);
	set_RGBi(0, 0, 0);
  hash += 1;
  speedLeft = 200;
  speedRight = 200;
	maneuver(speedLeft, speedRight, 20);       // Set wheel speeds

  delay(500);
		




} 



//XBEE code:


 

 

}

//Defines funtion 'rcTime' to read value from QTI sensor
// From Ch. 6 Activity 2 of Robotics with the BOE Shield for Arduino
long rcTime(int pin)
{
  pinMode(pin, OUTPUT);    // Sets pin as OUTPUT
  digitalWrite(pin, HIGH); // Pin HIGH
  delay(1);                // Waits for 1 millisecond
  pinMode(pin, INPUT);     // Sets pin as INPUT
  digitalWrite(pin, LOW);  // Pin LOW
  long time = micros();    // Tracks starting time
  while(digitalRead(pin)); // Loops while voltage is high
  time = micros() - time;  // Calculate decay time
  return time;             // Return decay time
}



// maneuver function
void maneuver(int speedLeft, int speedRight, int msTime)
{
  servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
  servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
  if(msTime==-1)                                   // if msTime = -1
  {                                  
    servoLeft.detach();                            // Stop servo signals
    servoRight.detach();   
  }
  delay(msTime);                                   // Delay for msTime
}



void set_RGBi(int r, int g, int b){
  // Set RGB LED pins based on high=bright 
  set_RGB(255-r, 255-g, 255-b);
}

void set_RGB(int r, int g, int b){
  // Set RGB LED pins based on low=bright (default)
  analogWrite(redpin, r);
  analogWrite(greenpin, g);
  analogWrite(bluepin, b);
}


char RFID() {
// RIFD code:

char val = 0; // variable to store the data from the serial port
int len = 12;

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

  if(strcmp(rfidData, "61003BF34DE4") == 0) {
  Serial.println("Found code");
  finalCode = hash;
  
  Serial2.print(String(finalCode)); // Send to XBee
  Serial.println("sending message");
  Serial.println(String(finalCode));
  
  return rfidData;

  }
  else {
  return rfidData;
  //mySerial.println(rfidData);
  }



}