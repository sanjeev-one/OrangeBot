
// Pins for QTI connections on board
#define lineSensor1 47 // left
#define lineSensor2 51 // middle
#define lineSensor3 52 // right

int hash = 1;		// Hashmark counter
char finalCode = 0; // Final code

// Define pins for built-in RGB LED
#define redpin 45
#define greenpin 46
#define bluepin 44

// LCD display
#include <SoftwareSerial.h>
#define TxPin 14
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

bool found = false;
bool receiving = false;
bool allHashesReceived = false;
int check[] = {0, 0, 0, 0, 0};

bool letterfound = false;

#include <Servo.h> // Include servo library

Servo servoLeft; // Declare left and right servos
Servo servoRight;

// array to wait for right letter to line up

char dictionary[] = {0, 0, 'a', 'b', 'c', 'd'};

// song stuff - can delete
#define nsong 17
int durs[nsong] = {211, 211, 211, 210, 210, 211, 211, 211, 211, 211, 211, 211, 210, 210, 211, 211, 212};
int octs[nsong] = {215, 215, 215, 215, 215,
				   215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215};
int notes[nsong] = {220, 220, 220, 220, 220,
					220, 225, 232, 220, 220, 220, 220, 220, 220, 220, 225, 232};

// end song stuff

void setup()
{
	Serial.begin(9600);	   // start the serial monitor so we can view the output
	servoLeft.attach(11);  // Attach left signal to pin 13
	servoRight.attach(12); // Attach right signal to pin 12
	maneuver(0, 0, 20);

	Serial.println("test");

	// RFID code & Serial:

	Serial1.begin(9600);  // connect to the serial port for the RFID reader
	mySerial.begin(9600); // lcd serial
	// mySerial.write(17); // backlight
	mySerial.write(21); // turn off lcd
	// XBEE serial start
	Serial2.begin(9600);
}
void loop()
{

	float qti1 = rcTime(lineSensor1); // left signal
	float qti2 = rcTime(lineSensor2); // middle signal
	float qti3 = rcTime(lineSensor3); // right signal

	float ndShade;						  // Normalized differential shade
	ndShade = qti3 / (qti1 + qti3) - 0.5; // Calculate it and subtract 0.5
										  // Serial.println(ndShade); // Print

	int speedLeft, speedRight; // Declare speed variables

	// slow down inner wheel:

	if (ndShade > 0.0) // Shade on right?
	{				   // Slow down left wheel
		speedRight = int(200.0 - (ndShade * 1000.0));
		speedRight = constrain(speedRight, -200, 200);
		speedLeft = 200; // Full speed right wheel
	}
	else // Shade on Left?
	{	 // Slow down right wheel
		speedLeft = int(200.0 + (ndShade * 1000.0));
		speedLeft = constrain(speedLeft, -200, 200);
		speedRight = 200; // Full speed left wheel
	}

	maneuver(speedLeft, speedRight, 20); // Set wheel speeds

	// delay(200);

	// Serial.print("left ");
	// Serial.println(qti1);
	// Serial.print("middle ");
	//  Serial.println(qti2);
	// Serial.print("right ");
	// Serial.println(qti3);

	// 1 is light

	int state = 4 * (qti1 < 500) + 2 * (qti2 < 500) + (qti3 < 500);

	// Serial.println(state);

	switch (state)
	{

	case 0: // hashmark
		speedLeft = 0;
		speedRight = 0;
		maneuver(speedLeft, speedRight, 20); // Set wheel speeds

		if (hash == 1)
		{
			// red color
			set_RGBi(255, 0, 0);
			finalCode = RFID();
		}
		if (hash == 2)
		{
			// yellow color
			set_RGBi(255, 255, 0);
			if (!found)
			{
				finalCode = RFID();
			}
		}
		if (hash == 3)
		{
			// green color
			set_RGBi(0, 255, 0);
			if (!found)
			{
				finalCode = RFID();
			}
		}
		if (hash == 4)
		{
			// blue color
			set_RGBi(0, 0, 255);
			if (!found)
			{
				finalCode = RFID();
			}
			// receiving = true;
			// Serial.println("receiving");
		}
		if (hash == 5)
		{
			// purple color
			set_RGBi(125, 0, 225);
			if (!found)
			{
				finalCode = RFID();
			}

			// final hash stuff - getting codes
			// maneuver(0, 0, 20); // freeze bot

			// Serial2.print(String(finalCode)); // Send to XBee

			// stop moving
			maneuver(0, 0, -1); // detach servos- freeze bot

			while (!allHashesReceived)
			{
				if (!((check[0] * check[1] * check[2] * check[3] * check[4]) == 120))
				{

					Serial2.print(finalCode); // Send to XBee
				}
				for (int i = 0; i < 4; i++)
				{
					// get other people's codes
					if (Serial2.available())
					{									// Is XBee data available?
						char recvHash = Serial2.read(); // Read character
						Serial.println(recvHash);

						/* Serial.println("sending message");
						Serial.println(String(finalCode)); */

						// delay(100);
						check[recvHash - 49] = int(recvHash - 48);

						Serial.println(dictionary[finalCode - 48]);
						Serial.println(dictionary[finalCode - 47]);
						Serial.println("dict^");
						if (recvHash == dictionary[finalCode - 48])
						{
							letterfound = true;
							Serial.println("letter found is now trueee");
						}

						mySerial.write(148); // cursor home line 1

						mySerial.print(String(check[0]) + " " + String(check[1]) + " " + " " + String(check[2]) + " " + String(check[3]) + " " + String(check[4]));
						Serial.println(String(check[0]) + " " + String(check[1]) + " " + " " + String(check[2]) + " " + String(check[3]) + " " + String(check[4]));
					}

					// CHECKING IF ALL HASHES ARE RECEIVED
					if (((check[0] * check[1] * check[2] * check[3] * check[4]) == 120) && (letterfound == true))
					{
						allHashesReceived = true;

						servoLeft.attach(11);  // Attach left signal to pin 13
						servoRight.attach(12); // Attach right signal to pin 12

						/* if (allHashesReceived)
						{
							Serial.println("All hashes received");
						} */
					}
				}

				delay(300);
			}
		}
		if (hash == 6)
		{
			// hash = 0;
			set_RGBi(0, 0, 0);

			/* while (1 == 1)
			{

				// Serial2.print(finalCode);   // Send to XBee
				// delay(100); //
				// Serial.println(finalCode);  //print to serial monitor
				blink(); // blink LED

				if (Serial2.available())
				{									// Is XBee data available?
					char recvHash = Serial2.read(); // Read character
					Serial.println(recvHash);		// Send to serial monitor
					check[int(recvHash) - 49] = int(recvHash) - 48;

					mySerial.write(148); // cursor home line 1

					mySerial.print(String(check[0]) + " " + String(check[1]) + " " + " " + String(check[2]) + " " + String(check[3]) + " " + String(check[4]));
					Serial.println(String(check[0]) + " " + String(check[1]) + " " + " " + String(check[2]) + " " + String(check[3]) + " " + String(check[4]));
				}
			} */
		}

		if (hash == 7)
		{

			speedLeft = 50;
			speedRight = -40;
			maneuver(speedLeft, speedRight, 20); // Set wheel speeds

			delay(20);
		}
		if (hash == 8)
		{

			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(200);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(500);
		}

		if (hash == 13 - (finalCode - 48))
		{
			maneuver(0, 0, 20);						   // Stop
													   //	if (((finalCode - 47) < 6))
													   //	{
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(100);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(200);
			Serial2.print(dictionary[finalCode - 47]); // Send to XBee
			delay(500);
			//	}

			while (1 == 1)
			{
			}
		}
		delay(1000);
		set_RGBi(0, 0, 0);
		hash += 1;
		speedLeft = 200;
		speedRight = 200;
		maneuver(speedLeft, speedRight, 20); // Set wheel speeds

		delay(500);
	}

	// XBEE code:
}

// Defines funtion 'rcTime' to read value from QTI sensor
//  From Ch. 6 Activity 2 of Robotics with the BOE Shield for Arduino
long rcTime(int pin)
{
	pinMode(pin, OUTPUT);	 // Sets pin as OUTPUT
	digitalWrite(pin, HIGH); // Pin HIGH
	delay(1);				 // Waits for 1 millisecond
	pinMode(pin, INPUT);	 // Sets pin as INPUT
	digitalWrite(pin, LOW);	 // Pin LOW
	long time = micros();	 // Tracks starting time
	while (digitalRead(pin))
		;					// Loops while voltage is high
	time = micros() - time; // Calculate decay time
	return time;			// Return decay time
}

// maneuver function
void maneuver(int speedLeft, int speedRight, int msTime)
{
	servoLeft.writeMicroseconds(1500 + speedLeft);	 // Set left servo speed
	servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
	if (msTime == -1)								 // if msTime = -1
	{
		servoLeft.detach(); // Stop servo signals
		servoRight.detach();
	}
	else
	{
		delay(msTime); // Delay for msTime
	}
}

void set_RGBi(int r, int g, int b)
{
	// Set RGB LED pins based on high=bright
	set_RGB(255 - r, 255 - g, 255 - b);
}

void set_RGB(int r, int g, int b)
{
	// Set RGB LED pins based on low=bright (default)
	analogWrite(redpin, r);
	analogWrite(greenpin, g);
	analogWrite(bluepin, b);
}

char RFID()
{
	// RIFD code:

	char val = 0; // variable to store the data from the serial port
	int len = 12;

	char rfidData[len + 1] = {};
	int get_more = 1;
	int i = 0;

	while (get_more == 1)
	{
		if (Serial1.available() > 0)
		{
			val = Serial1.read();

			// Handle unprintable characters
			switch (val)
			{
			case 0x2:
				break; // start of transmission - do not save
			case 0x3:
				get_more = 0;
				break; // end of transmission - done with code
			case 0xA:
				break; // line feed - do not save
			case 0xD:
				break; // carriage return - do not save
			default:
				rfidData[i] = val;
				i += 1;
				break; // actual character
			}
		}
	}
	Serial.println(rfidData);

	if (strcmp(rfidData, "61003BF34DE4") == 0)
	{
		Serial.println("Found code");
		finalCode = char(hash + 48);
		found = true;
		check[hash - 1] = int(hash);

		// led stuff
		delay(500);
		blink();

		// lcd stuff
		mySerial.write(24); // cursor on
		delay(10);
		// mySerial.write(17);  // backlight on
		mySerial.write(128); // cursor home line 0
		delay(10);
		mySerial.print("Position: ");
		delay(10);
		mySerial.print(hash);
		delay(10);
		// mySerial.write(18);  // backlight off

		found = true;
		return finalCode;
	}
	else
	{
		return;
		// mySerial.println(rfidData);
	}
}

void play_song()
{
	for (long k = 0; k < nsong; k++)
	{

		mySerial.write(durs[k]);
		mySerial.write(octs[k]);
		mySerial.write(notes[k]);
		int len = 214 - durs[k];
		float del = 2000 / pow(2, len);
		delay(int(del * 1.1));
	}
}
void blink()
{
	// led stuff
	// delay(500);
	set_RGBi(0, 0, 0);
	delay(500);
	set_RGBi(255, 0, 0);
	delay(500);
	set_RGBi(0, 0, 0);
}