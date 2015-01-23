// verstuur vraag nummer naar slaves:
http://arduino.cc/en/Tutorial/MasterWriter

#include <Wire.h>

// ------------ master code: --------------------------------------------------------------------

void setup () {
	wire.begin(); // join i2c bus (address optional for master)
	
	byte scoreArray[3] = {0, 0, 0};
	byte questionIndex = 2;
	byte rondeNummer = 4;
	byte maxRondes = 20;
	byte spelStatus = 1;
}

void loop () {
	transmitQuestion(questionIndex);
	transmitRound(rondeNummer, maxRondes);
	transmitScore(scoreArray);
	transmitGameStatus(spelStatus);
	delay(10000);
}


void transmitQuestion(int questionIndex) { //stuurt vraag nummer naar slaves
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(1); //geeft aan dat het om de vraag gaat (1)
		wire.write(questionIndex); // send 1 bytes (vraag nummer)
		wire.endTransmission(); // stop transmitting
	}
}

void transmitRound(int rondeNummer, int maxRondes) { //stuurt ronde nummer en max aantal rondes naar slaves
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(2); //geeft aan dat het om de ronde info gaat (2)
		wire.write(rondeNummer); // send 1 bytes (huidige ronde nummer)
		wire.write(maxRondes); // send 1 bytes (maximum aantal rondes)
		wire.endTransmission(); // stop transmitting
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

void transmitScore(int *scoreArray) { //stuurt score array naar slaves
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(5); //geeft aan dat het om de score gaat (5)
		wire.write(scoreArray, 3); // send score array (3 bytes)
		wire.endTransmission(); // stop transmitting
	}
}

void transmitGameStatus(char[5] spelStatus) { //stuurt spel status naar slaves (true of false, volgende ronde of spel afgelopen?)
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(6); //geeft aan dat het om de spelstatus gaat (6)
		wire.write(spelStatus); // send 1 bytes (0 of 1)
		wire.endTransmission(); // stop transmitting
	}
}



