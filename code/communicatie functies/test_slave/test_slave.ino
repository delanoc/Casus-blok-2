// verstuur vraag nummer naar slaves:
//http://arduino.cc/en/Tutorial/MasterWriter

#include <Wire.h>

// ----------- slave code: ------------------------------------------------------------

//globale variabelen:
int questionIndex;
int rondeNummer;
int maxRondes;
int scoreArray(3);
int spelStatus;

int gamemodeID = 1;

char antwoord[] = "D";
char reactietijd[] = "1740";

void setup()
{
	Serial.begin(9600);
	Serial.print("initialized");

	Wire.begin(3);             // join i2c bus with address #2
	Wire.onReceive(ontvanger); // register event
        Wire.onRequest(antwoorden);

        
        
}

void loop()
{
   delay(100);
}




// function that executes whenever data is received from master
void ontvanger(int numBytes)
{
        int k = Wire.available();
        Serial.print(k);
	//lees eerste write (byte 1 t/m 5, geeft communicatie type aan)
        Serial.print("\n start ontvangen, type: ");
	int type = Wire.read();
	Serial.print(type);


	if (type == 1) { // gaat om vraag nummer
                int questionIndex = Wire.read();   // receive byte as an integer
		Serial.println("\n vraag nummer: "); Serial.println(questionIndex);      // print the integer
	}
	else if (type == 2) { // gaat om ronde info
		Serial.println("\n ronde nummer: ");
                int rondeNummer = Wire.read();
		Serial.println(rondeNummer);
        }
        else if (type == 3) { // gaat om ronde info
		Serial.println("\n max aantal rondes: ");
		int maxRondes = Wire.read();
		Serial.println(maxRondes);
	}
	else if (type == 4) { // gaat om score info
                int scoreArray[3];
		while (Wire.available() > 0) {
			for (int x=0; x < 3; x++) {
				int value = Wire.read();
                                scoreArray[x] = value;
			}
		}
		Serial.println("\n score array: "); 
		for (int x=0; x < 3; x++) {
                  Serial.println(scoreArray[x]);
                }
	}
	else if (type == 5) { // gaat om spel status
		int spelStatus = Wire.read();
		Serial.println("\n spelstatus: "); Serial.println(spelStatus);
	}

}


void antwoorden()
{
    //gamemodeID, 1= meerkeuze, 0=buzzer, moet zelfde zijn als master code

    if (gamemodeID == 1) { //in geval van meerkeuze:
      Wire.write(antwoord);
    }
    else if (gamemodeID == 0) { //in geval van buzzer:
      Wire.write(reactietijd);
    }
    
}
