/*
 Robotics with the BOE Shield – BothServosStayStill
 Generate signals to make the servos stay still for centering.
 */

#include <Servo.h>                           // Include servo library
 
Servo servoLeft;                             // Declare left servo signal
Servo servoRight;                            // Declare right servo signal

void setup()                                 // Built in initialization block
{ 
  servoLeft.attach(12);                      // Attach left signal to P11 
  servoRight.attach(11);                      // Attach left signal to P12 

  servoLeft.writeMicroseconds(1500);         // 1.5 ms stay still sig, pin 11
  servoRight.writeMicroseconds(1500);  
  
  
   servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1300);  
  delay(620);
  servoRight.writeMicroseconds(1700);
  servoLeft.writeMicroseconds(1700);    
  delay(650);
   servoRight.writeMicroseconds(1375);
   servoLeft.writeMicroseconds(1700);  
   delay(11000); 
   servoLeft.writeMicroseconds(1500);         // 1.5 ms stay still sig, pin 11
  servoRight.writeMicroseconds(1500);  
   
         // 1.5 ms stay still sig, pin 12
}  
 
void loop()                                  // Main loop auto-repeats
{           
   
  
}            

