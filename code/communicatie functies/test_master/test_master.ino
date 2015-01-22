// verstuur vraag nummer naar slaves:
//http://arduino.cc/en/Tutorial/MasterWriter

#include <Wire.h>

// ------------ master code: --------------------------------------------------------------------

void setup () {
  Serial.begin(9600);
  Serial.print("initialized");
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
  
  
  //1/0 = gamemode ID, moet zelfde zijn als op slave
  //getAntwoorden(1); delay(1000);
  //getAntwoorden(0); delay(1000);
  
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
  for (byte x = 2; x < 5; x++) { //stuur het volgende aan device 2, 3 en 4
    Wire.beginTransmission(x); //stuur het volgende aan device 2, 3 en 4
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
	for (byte x = 2; x < 5; x++) { //stuur het volgende aan device 2, 3 en 4
	  Wire.beginTransmission(x); // transmit to device #x
	  Wire.write(4); //geeft aan dat het om de score gaat (4)
	  Wire.write(scoreArray, 3); // send score array (3 bytes)
	  Wire.endTransmission(); // stop transmitting
	}
}

void transmitGameStatus(byte spelStatus) { //stuurt spel status naar slaves (true of false, volgende ronde of spel afgelopen?)
	for (byte x = 2; x < 5; x++) { //stuur het volgende aan device 2, 3 en 4
	  Wire.beginTransmission(x); // transmit to device #x
	  Wire.write(5); //geeft aan dat het om de spelstatus gaat (5)
	  Wire.write(spelStatus); // send 1 bytes (0 of 1)
  	  Wire.endTransmission(); // stop transmitting
	}
}


void getAntwoorden(int gamemodeID) { //vraagt slaves om antwoord op vraag
    //gamemodeID, 1= meerkeuze, 0=buzzer
    
    char meerkeuzeAntwoorden[3]; // sla het antwoord van de speler op als een char (deze is "A", "B", "C" of "D")
    int reactietijdenBuzzer[3]; // sla de reactietijden op in een array als integers
    
    for (int y = 0; y < 3; y++) {
      
      if (gamemodeID == 1) { //in geval van meerkeuze:
        
        if (y == 0) { Wire.requestFrom(2, 1); }    // request 1 bytes from slave device #2
        else if (y == 1) { Wire.requestFrom(3, 1); }    // request 1 bytes from slave device #2
        else if (y == 2) { Wire.requestFrom(4, 1); }    // request 1 bytes from slave device #2
        
        meerkeuzeAntwoorden[y] = Wire.read(); //sla de byte op
        delay(200);
        Serial.print("\n\n Antwoord speler: "); Serial.print(meerkeuzeAntwoorden[y]);
      }
      
      else if (gamemodeID == 0) { //in geval van buzzer:
        char inputSpeler[4]; // sla het onvangen bericht tijdelijk op als een string van 4 characters
        int tijd = 0; // sla reactietijd op als integer
        
        if (y == 0) { Wire.requestFrom(2, 1); }    // request 1 bytes from slave device #2
        else if (y == 1) { Wire.requestFrom(3, 1); }    // request 1 bytes from slave device #2
        else if (y == 2) { Wire.requestFrom(4, 1); }    // request 1 bytes from slave device #2
        
        Wire.requestFrom(2, 4);    // request 4 bytes from slave device #2
        Serial.print("\n voor: "); Serial.print(inputSpeler);
        for (int x = 0; x < 4; x++) {
          inputSpeler[x] = Wire.read(); //sla de bytes op als characters
          tijd = 10 * tijd + inputSpeler[x] - 48; //reken de string om naar een integer (bv: string "1740" wordt een integer met waarde 1740)
          Serial.print(tijd); Serial.print(" ");
          //Serial.print("\n read: "); Serial.print(inputSpeler[x]);
        }
        reactietijdenBuzzer[y] = tijd;
        Serial.print("\n Reactietijd speler 1 char: "); Serial.print(inputSpeler);
        Serial.print("\n Reactietijd speler 1 int: "); Serial.print(tijd);//Serial.print(reactietijdSpeler1);
        delay(200);
      }
      
    }
    
}

