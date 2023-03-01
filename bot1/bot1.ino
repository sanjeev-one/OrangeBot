void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial2.begin(9600);
delay(500);
}


void loop() {
  // put your main code here, to run repeatedly:


if(Serial.available()) { // Is serial monitor data available?
  char outgoing = Serial.read(); // Read character
  Serial2.print(outgoing); // Send to XBee
}

if(Serial2.available()) { // Is XBee data available?
  char incoming = Serial2.read(); // Read character
  Serial.println(incoming); // Send to serial monitor
}
delay(50);



}
