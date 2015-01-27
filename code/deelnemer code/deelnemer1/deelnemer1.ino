/*
	Namen: 			Delano Cörvers, Camiel Kerkhofs, Dennis Kooij, Davy Heutmekers
 	Studentnummers:	1306669corvers, 1163981kooij, 1331833kerkhofs, 1309730heutmekers
 	Klas:			IT1
 	Module:			PIT2 (2014-2015)
 	Versie:			1
 	Inleverdatum:	2 februari 2015
 */

// ---------- Includes
#include <LiquidCrystal.h> // LCD Library
#include <Wire.h> // Transmission library
#include <string.>

// ---------- Constants
#define VERSION		1

// Buttons
#define btnPrev		5 // Previous button for the lcd menu
#define btnNext		4 // Next button for the lcd menu
#define btnSelect	3 // Select button for the lcd menu
#define btnToggle	2 // Toggle button for the score

// ---------- Variables
// LCD Screen
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LCD pins

int buzzer = 9;
int ledlamp = 13;

//globale variabelen:
byte questionIndex = 0;
byte rondeNummer;
byte maxRondes = 0;
byte scoreArray(3);
byte spelStatus= 0;
byte gamemodeID=0;
byte huidigeRonde = 0;
char antwoord[] = "X";
char reactietijd[] = "0000";



//antwoord mogelijkheden
const char* antwoordenArray[][4] = {
  {"A: Utrecht", "B: Amsterdam", "C: Maastricht", "D: Groningen"},
  {"A: 1", "B: 2", "C: 3", "D: 4"},
  {"A: Amalia", "B: Willem V", "C: Alexia", "D: Ariane"},
  {"A: Henkjan", " B: Frank", "C: Claus", "D: Willem"},
  {"Onjuist", "Juist", "-", "-"},
  {"Onjuist", "Juist", "-", "-"},
  {"A: Microsoft", "B: Apple", "C: Ikea", "D: Huaweii"},
  {"A: E Watson", "B: R Weasley", "C: D Radcliffe", "D: R Pattin"},
  {"A: Basketbal", "B: Hockey", "C: Voetbal", "D: Korfbal"},
  {"A: 1455", "B: 2001", "C: 1997", "D 1981"},
  {"A: 8", "B: 10", "C: 12", "D: 11"},
  {"A: 16", "B: 7", "C: 11", "D: 9"},
  {"A: 24", "C: 12", "C: 16", "D: 8"},
  {"A: 8", "B: 4", "C: 6", "D: 2"},
  {"A: Rood", "B: Geel", "C: Groen", "D: Oranje"},
  {"A: 49", "B: 64", "C: 81", "D: 100"},
  {"A: Thermometer", "B: Barometer", "C: Magnetometer", "D: Seismograaf"},
  {"A: L Hemsworth", "B: J Lawrence", "C: E Banks", "D: Stanley Tucci"},
  {"A: Monaco", "B: Kiev", "C: Rome", "D: Minsk"},
  {"A: Aanraking", "B: Bakstenen", "C: Insecten", "D: Spoken"}
};



void setup()
{
  pinMode(btnPrev, INPUT);
  pinMode(btnNext, INPUT);
  pinMode(btnSelect, INPUT);
  pinMode(btnToggle, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(ledlamp, OUTPUT);

  Serial.begin(9600);
  Serial.print("initialized");

  Wire.begin(2);             // join i2c bus with address #2
  Wire.onReceive(ontvanger); // register event
  Wire.onRequest(antwoorden);

  lcd.begin(16,2);
  lcd.clear(); 
  lcd.print("welkom"); 
  lcd.setCursor(0, 1); 
  lcd.print("wacht op start..");
}

void loop()
{
  boolean startGame = false;
  while (!startGame) {
    if (rondeNummer > huidigeRonde) {
      startGame = true;
    }
    delay(100);
  }
  huidigeRonde = huidigeRonde + 1;

  // Welcome message
  lcd.clear(); lcd.print("Ronde begint..");
  lcd.setCursor(0, 1); lcd.print("Ronde: "); lcd.print(rondeNummer);
  delay(3000);
  
  if (questionIndex < 20) {
    gamemodeID = 1;
  } // meerkeuze vraag
  else if (questionIndex > 19) {
    gamemodeID = 0;
  } // buzzer vraag

  Serial.println("\ngamemodeID: "); Serial.print(gamemodeID);

  if (gamemodeID == 1) {
    beantwoordMeerkeuze();
  }

  else if (gamemodeID == 0) {
    beantwoordBuzzer();
  }

}


void beantwoordMeerkeuze()
{
  int printIndex = 1;
  boolean selected = false;

  while (!selected) {
    boolean pressed = false;
    while (!pressed) {
      if (digitalRead(btnPrev) == HIGH) {
        if (printIndex > 1) {
          int printIndex = printIndex -1;
        }
        pressed = true;
      }
      else if (digitalRead(btnNext) == HIGH) {
        if (printIndex < 4) {
          int printIndex = printIndex +1;
        }
        pressed = true;
      }
      else if (digitalRead(btnSelect) == HIGH) {
        pressed = true;
        selected = true;
      }
      lcd.clear(); 
      lcd.print("antwoord:"); 
      lcd.setCursor(0, 1); 
      lcd.print(antwoordenArray[questionIndex][printIndex]);
      delay(200);
    }
  }
  if (printIndex == 1) {
    char antwoord[] = "A";
  }
  else if (printIndex == 2) {
    char antwoord[] = "B";
  }
  else if (printIndex == 3) {
    char antwoord[] = "C";
  }
  else if (printIndex == 4) {
    char antwoord[] = "D";
  }
}


void beantwoordBuzzer()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("druk om te"); 
  lcd.setCursor(0, 1); 
  lcd.print("antwoorden");

  int responseTimeStart = millis();
  int pressed = 0;
  int responseTimeEnd;
  while (pressed == 0) {
    if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (digitalRead(btnSelect) == HIGH) || (digitalRead(btnToggle) == HIGH)) {
      pressed = 1;
      responseTimeEnd = millis();
      lcd.clear(); 
      lcd.print("gedrukt!");
    }
  }
  int reactietijdInteger  = responseTimeEnd - responseTimeStart;
  reactietijdInteger = reactietijdInteger + 1000;
  String reactietijdString = String(reactietijdInteger);
  reactietijdString.toCharArray(reactietijd,4);
}


// function that executes whenever data is received from master
void ontvanger(int numBytes)
{
  //lees eerste write (byte 1 t/m 5, geeft communicatie type aan)
  Serial.print("\n start ontvangen, type: ");
  int type = Wire.read();
  Serial.print(type);

  if (type == 1) { // gaat om vraag nummer
    questionIndex = Wire.read();   // receive byte as an integer
    Serial.println("\n vraag nummer: "); 
    Serial.println(questionIndex);
  }
  else if (type == 2) { // gaat om ronde info
    Serial.println("\n ronde nummer: ");
    rondeNummer = Wire.read();
    Serial.println(rondeNummer);
  }
  else if (type == 3) { // gaat om ronde info
    Serial.println("\n max aantal rondes: ");
    maxRondes = Wire.read();
    Serial.println(maxRondes);
  }
  else if (type == 4) { // gaat om score info
    byte scoreArray[3];
    while (Wire.available() > 0) {
      for (int x=0; x < 3; x++) {
        byte value = Wire.read();
        scoreArray[x] = value;
      }
    }
    Serial.println("\n score array: "); 
    for (int x=0; x < 3; x++) {
      Serial.println(scoreArray[x]);
    }
  }
  else if (type == 5) { // gaat om spel status
    spelStatus = Wire.read();
    Serial.println("\n spelstatus: "); 
    Serial.println(spelStatus);
  }
  else if (type == 6) { // gaat om buzzer status
    digitalWrite(buzzer, HIGH);
    digitalWrite(ledlamp, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
    digitalWrite(ledlamp, LOW);
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


