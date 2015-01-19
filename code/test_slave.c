// verstuur vraag nummer naar slaves:
http://arduino.cc/en/Tutorial/MasterWriter

#include <Wire.h>

// ----------- slave code: ------------------------------------------------------------



void setup()-
{
	Serial.begin(9600);
	
	Wire.begin(2);                // join i2c bus with address #2
	Wire.onReceive(ontvanger); // register event
}

void loop()
{
	delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void ontvanger(int numBytes)
{
	
	int type = Wire.read()
	
	//lees eerste write (integer 1 t/m 6, geef communicatie type aan)
	// type = eerste write ??
	if (type == 1) { // gaat om vraag nummer
		int questionIndex = Wire.read();   // receive byte as an integer
		Serial.println("\n vraag nummer: "); Serial.println(questionIndex);      // print the integer
	}
	else if (type == 2) { // gaat om ronde info
		int rondeNummer = Wire.read();
		int maxRondes = Wire.read();
		Serial.println("\n ronde nummer / max aantal rondes: "); Serial.println(rondeNummer); Serial.println(" / "); Serial.println(maxRondes);
	}
	else if (type == 5) { // gaat om score info
		while (Wire.available() > 0) {
			for (int x=0, x < 3, x++) {
				int scoreArray[x] = Wire.read():
			}
		}
		Serial.println("\n score array: "); 
		for (int x=0, x < 3, x++) { Serial.println(scoreArray[x]); }
	}
	else if (type == 6) { // gaat om spel status
		int spelStatus = Wire.read();
		Serial.println("\n spelstatus: "); Serial.println(spelStatus);
	}
	

}