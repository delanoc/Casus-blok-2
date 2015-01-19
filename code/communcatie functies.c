// verstuur vraag nummer naar slaves:
http://arduino.cc/en/Tutorial/MasterWriter



// ------------ master code: --------------------------------------------------------------------

void setup () {
	wire.begin(); // join i2c bus (address optional for master)
}

void loop () {
	int questionIndex = 2;
	char[16] tussenstand = "1:00  2:00  3:00"
	char[5] spelStatus = "true "
	
	transmitQuestion(questionIndex);
	transmitRound(5, 20);
	
	transmitScore(tussenstand);
	transmitGameStatus(spelStatus);

}

void transmitQuestion(int questionIndex) { //stuurt vraag nummer naar slaves
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(questionIndex); // send 2 bytes (integer)
		wire.endTransmission(); // stop transmitting
	}
}

void transmitRound(int rondeNummer, int maxRondes) { //stuurt ronde nummer en max aantal rondes naar slaves
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(rondeNummer); // send 2 bytes (integer)
		wire.write(maxRondes); // send 2 bytes (integer)
		wire.endTransmission(); // stop transmitting
	}
}

void getAntwoorden() { //vraagt slaves om antwoorden op meerkeuze vraag
}

void getReactietijden() { //vraagt slaves om reactietijden bij buzzer vraag
}

void transmitScore(char[16] tussenstand;) { //stuurt nieuwe tussenstand naar slaves
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(tussenstand); // send 16 bytes (16 character string)
		wire.endTransmission(); // stop transmitting
	}
}

void transmitGameStatus(char[5] spelStatus) { //stuurt spel status naar slaves (true of false, volgende ronde of spel afgelopen?)
	for (int x = 2, x < 5, x++) {
		wire.beginTransmission(x); // transmit to device #x
		wire.write(spelStatus); // send 5 bytes (5 character string) ("true " of "false"
		wire.endTransmission(); // stop transmitting
	}
}






// ----------- slave code: ------------------------------------------------------------



void setup()-
{
	Serial.begin(9600);
	Wire.begin(2);                // join i2c bus with address #2
	Wire.onReceive(ontvanger); // register event
}

void loop()
{
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void ontvanger(int numBytes)
{
  int questionIndex = Wire.read();   // receive byte as an integer
  Serial.println(questionIndex);         // print the integer
}