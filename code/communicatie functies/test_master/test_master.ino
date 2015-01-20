// verstuur vraag nummer naar slaves:
//http://arduino.cc/en/Tutorial/MasterWriter

#include <Wire.h>

// ------------ master code: --------------------------------------------------------------------

void setup () {
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop () {
  
  byte scoreArray[3] = {0, 0, 0};
  byte questionIndex = 200;
  byte roundNumber = 201;
  byte roundMax = 202;
  byte spelStatus = 1;
  
  transmitQuestion(questionIndex); delay(1000);
  transmitRoundNum(roundNumber); delay(1000);
  transmitRoundMax(roundMax); delay(1000);
  transmitScore(scoreArray); delay(1000);
  transmitGameStatus(spelStatus); delay(1000);
  
  delay(1000);
  
}


void transmitQuestion(byte questionIndex) { //stuurt vraag nummer naar slaves
  for (byte x = 2; x < 5; x++) {
    Wire.beginTransmission(x); // transmit to device #x
    Wire.write(1); //geeft aan dat het om de vraag gaat (1)
    Wire.write(questionIndex); // send 1 bytes (vraag nummer)
    Wire.endTransmission(); // stop transmitting
  }
}

void transmitRoundNum(byte roundNumber) {
  for (byte x = 2; x < 5; x++) {
    Wire.beginTransmission(x);
    Wire.write(2);
    Wire.write(roundNumber);
    Wire.endTransmission();
  }
}

void transmitRoundMax(byte roundMax) {
  for (byte x = 2; x < 5; x++) {
    Wire.beginTransmission(x);
    Wire.write(3);
    Wire.write(roundMax);
    Wire.endTransmission();
  }
}

void transmitScore(byte *scoreArray) { //stuurt score array naar slaves
	for (byte x = 2; x < 5; x++) {
	  Wire.beginTransmission(x); // transmit to device #x
	  Wire.write(4); //geeft aan dat het om de score gaat (4)
	  Wire.write(scoreArray, 3); // send score array (3 bytes)
	  Wire.endTransmission(); // stop transmitting
	}
}

void transmitGameStatus(byte spelStatus) { //stuurt spel status naar slaves (true of false, volgende ronde of spel afgelopen?)
	for (byte x = 2; x < 5; x++) {
	  Wire.beginTransmission(x); // transmit to device #x
	  Wire.write(5); //geeft aan dat het om de spelstatus gaat (5)
	  Wire.write(spelStatus); // send 1 bytes (0 of 1)
  	  Wire.endTransmission(); // stop transmitting
	}
}

/*
void getAntwoorden() { //vraagt slaves om antwoorden op meerkeuze vraag
//request eerst
}

void getReactietijden() { //vraagt slaves om reactietijden bij buzzer vraag
//request eerst
}
*/

