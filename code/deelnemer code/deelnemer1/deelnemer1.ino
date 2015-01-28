/*
	Namen: 			Delano Cörvers, Camiel Kerkhofs, Dennis Kooij, Davy Heutmekers
 	Studentnummers:	1306669corvers, 1163981kooij, 1331833kerkhofs, 1309730heutmekers
 	Klas:			IT1
 	Module:			PIT2 (2014-2015)
 	Versie:			deelnemer.1
 	Inleverdatum:	2 februari 2015
*/


// ---------- Includes
#include <LiquidCrystal.h> //LCD Library
#include <Wire.h> //I2C communication library
#include <string.>

//hardware pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //LCD

int buzzer = 9; //buzzer
int ledlamp = 13; //led lampje

int btnPrev = 5; //knop 1 (vorige)
int btnNext = 4; //knop 2 (volgende)
int btnSelect = 3; //knop 3 (selecteer keuze)
int btnToggle = 2; //knop 4 (scores weergeven)
 
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

//antwoord mogelijkheden meerkeuze vragen:
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

  Wire.begin(2); // sluit aan op I2C bus met adres 2
  Wire.onReceive(ontvanger); // gebruik 'ontvanger' functie als er iets gestuurd word door de master
  Wire.onRequest(antwoorden); // gebruik 'antwoorden' functie als er iets gevraagd word door de master

  lcd.begin(16,2); //initialiseer lcd scherm
  lcd.clear(); lcd.print("welkom"); lcd.setCursor(0, 1); lcd.print("wacht op start.."); //welkomstbericht
}


void loop()
{
  //begin nieuwe ronde zodra quizmaster arduino het nieuwe ronde nummer doorstuurt
  boolean startGame = false;
  while (!startGame) {
    if (rondeNummer > huidigeRonde) {
      startGame = true;
    }
    delay(100);
  }
  huidigeRonde = huidigeRonde + 1;

  lcd.clear(); lcd.print("Ronde begint..");
  lcd.setCursor(0, 1); lcd.print("Ronde: "); lcd.print(rondeNummer);
  delay(3000); //geef quizmaster arduino tijd om questionIndex door te sturen.
  
  
  if (questionIndex < 20) {
	gamemodeID = 1; // meerkeuze vraag
	beantwoordMeerkeuze(); //start functie om antwoord te geven op de meerkeuze vraag
  } 
  
  else if (questionIndex > 19) {
	gamemodeID = 0; // buzzer vraag
	beantwoordBuzzer(); //start functie om antwoord te geven op de buzzer vraag
  } 

  //ronde afgelopen
  //score weergeven?
  //delay?
}


// -------- functies:

//functie voor het beantwoorden van de meerkeuze vraag:
void beantwoordMeerkeuze()
{
  //laat gebruiker door de antwoord mogelijkheden scrollen met buttons.
  //printIndex 1 = antwoord A, printIndex 2 = antwoord B, enzovoorts.
  lcd.clear();
  int printIndex = 1;
  boolean selected = false;
  while (!selected) { //wacht tot gebruiker een definitieve keuze maakt
    boolean pressed = false;
    while (!pressed) { //wacht tot gebruiker een button indrukt
      if (digitalRead(btnPrev) == HIGH) {
        if (printIndex > 1) {
          int printIndex = printIndex -1; //ga naar vorige antwoord
        }
        pressed = true;
      }
      else if (digitalRead(btnNext) == HIGH) {
        if (printIndex < 4) {
          int printIndex = printIndex +1; //ga naar volgende antwoord
        }
        pressed = true;
      }
      else if (digitalRead(btnSelect) == HIGH) {
        pressed = true;
        selected = true; //het huidige antwoord wordt geselecteerd als definitief
      }
	  //print het huidige antwoord:
	  lcd.setCursor(0, 0); lcd.print("antwoord:"); 
      lcd.setCursor(0, 1); lcd.print(antwoordenArray[questionIndex][printIndex]);
    }
	delay(200); //delay voorkomt dubbele button klik
  }
  
  //sla het gekozen antwoord op als een char van 1 byte
  if (printIndex == 1) {char antwoord[] = "A";}
  else if (printIndex == 2) {char antwoord[] = "B";}
  else if (printIndex == 3) {char antwoord[] = "C";}
  else if (printIndex == 4) {char antwoord[] = "D";}
}



//functie voor het beantwoorden van de buzzer vraag:
void beantwoordBuzzer()
{
  int responseTimeStart = millis(); //meet begin tijd
  lcd.clear(); lcd.print("druk om te"); 
  lcd.setCursor(0, 1); lcd.print("antwoorden..");
  
  //wacht op button press van gebruiker
  boolean pressed = false;
  int responseTimeEnd;
  while (!pressed) {
    if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (digitalRead(btnSelect) == HIGH) || (digitalRead(btnToggle) == HIGH)) {
      pressed = true;
      responseTimeEnd = millis(); //meet eind tijd
      lcd.clear(); lcd.print("gedrukt!");
    }
  }
  
  int reactietijdInteger  = responseTimeEnd - responseTimeStart; //bereken reactietijd
  reactietijdInteger = reactietijdInteger + 1000; //zorgt dat reactietijd minimaal 4 bytes heeft
  String reactietijdString = String(reactietijdInteger); //sla integer op als string
  reactietijdString.toCharArray(reactietijd,4); //sla string op als character array met lengte 4 (bv '1740')
}



//functie voor het ontvangen van berichten van de quizmaster arduino
void ontvanger(int numBytes)
{
  //lees eerste write (byte 1 t/m 6, geeft communicatie type aan)
  Serial.print("\n start ontvangen, type: ");
  int type = Wire.read();
  Serial.print(type);

  if (type == 1) { // gaat om vraag nummer
	//sla de vraag index van de huidige ronde op 
	//(0 t/m 19 = meerkeuze vraag, 20 t/m 39 = buzzer vraag)
    questionIndex = Wire.read();   // receive byte as an integer
    Serial.println("\n vraag nummer: "); 
    Serial.println(questionIndex);
  }
  
  else if (type == 2) { // gaat om ronde info
	//sla het huidige rondenummer op
    Serial.println("\n ronde nummer: ");
    rondeNummer = Wire.read();
    Serial.println(rondeNummer);
  }
  
  else if (type == 3) { // gaat om ronde info
    //sla het maximaal aantal te spelen rondes op
	Serial.println("\n max aantal rondes: ");
    maxRondes = Wire.read();
    Serial.println(maxRondes);
  }
  
  else if (type == 4) { // gaat om score info
	//sla de score array op met de nieuwe tussenstand
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
    //spelstatus is 1 of 0 en geeft aan of het spel is afgelopen (1 = nieuwe ronde, 0 = afgelopen)
	spelStatus = Wire.read(); 
    Serial.println("\n spelstatus: "); 
    Serial.println(spelStatus);
  }
  
  else if (type == 6) { // gaat om buzzer status
    //laat buzzer en led 2 seconden aangaan als quizmaster een type 6 stuurt
	digitalWrite(buzzer, HIGH); 
    digitalWrite(ledlamp, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
    digitalWrite(ledlamp, LOW);
  }
}

//functie voor doorsturen van de antwoorden als quizmaster hierom vraagt.
void antwoorden()
{
  if (gamemodeID == 1) { //in geval van meerkeuze: (1 byte)
    Wire.write(antwoord);
  }
  else if (gamemodeID == 0) { //in geval van buzzer: (4 bytes)
    Wire.write(reactietijd);
  }    
}

