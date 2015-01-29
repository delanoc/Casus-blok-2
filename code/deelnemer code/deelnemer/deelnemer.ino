/*
	Namen: 			Delano Cörvers, Camiel Kerkhofs, Dennis Kooij, Davy Heutmekers
 	Studentnummers:	        1306669corvers, 1163981kooij, 1331833kerkhofs, 1309730heutmekers
 	Klas:			IT1
 	Module:			PIT2 (2014-2015)
 	Versie:			deelnemer.1
 	Inleverdatum:	        2 februari 2015
*/

//-------- deelnemer nummer: --------
//deze variabele is de enige die aangepast wordt tijdens het uploaden naar de deelnemers
//iedere deelnemer arduino heeft een ander ID
//deelnemerID kan zijn 1, 2 of 3.
int deelnemerID = 1;
//-----------------------------------


//Includes:
#include <LiquidCrystal.h> //LCD Library
#include <Wire.h> //I2C communication library
#include <string.> //voor omzetten integer naar string naar char

//hardware pins:
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //LCD
int buzzer = 6; //buzzer
int ledLamp = 13; //led lampje
int btnPrev = 5; //knop 1 (vorige)
int btnNext = 4; //knop 2 (volgende)
int btnSelect = 3; //knop 3 (selecteer keuze)
int btnToggle = 2; //knop 4 (scores weergeven)
 
 
//globale variabelen:
byte questionIndex = 0; //de vraag index (voor weergeven antwoord mogelijkheden en bepalen van gamemodeID)
byte rondeNummer = 0; //huidige ronde nummer van quizmaster arduino
byte maxRondes = 0; //maximaal aantal te spelen rondes
byte scoreArray[3]; //huidige scores van de 3 deelnemers
byte spelStatus = 1; //geeft aan of het spel is afgelopen of doorgaat (nieuwe ronde)
byte gamemodeID = 0; //geeft aan of het een buzzer ronde (=0) of meerkeuze ronde(=1) is
byte huidigeRonde = 0; //huidige ronde nummer van deelnemer arduino
byte printIndex = 0; //het huidige geslecteerde meerkeuze antwoord (0 = A, 1 = B, enz.)
char reactietijd[] = "9999"; //reactietijd buzzer knop
boolean bezig = true; //status van toggle menu (true = kan togglen, false = kan niet togglen)


//antwoord mogelijkheden meerkeuze vragen:
const char* antwoordenArray[][4] = {
  {"A: Utrecht", "B: Amsterdam", "C: Maastricht", "D: Groningen"},
  {"A: 1", "B: 2", "C: 3", "D: 4"},
  {"A: Amalia", "B: Willem V", "C: Alexia", "D: Ariane"},
  {"A: Henkjan", " B: Frank", "C: Claus", "D: Willem"},
  {"Juist", "Onjuist", "-", "-"},
  {"Juist", "Onjuist", "-", "-"},
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
  {"A: Tom Cruise", "B: Brad Pitt", "C: Johnny Depp", "D: Liam Neeson"},
  {"A: Aanraking", "B: Bakstenen", "C: Insecten", "D: Spoken"},
  {"A: Monaco", "B: Kiev", "C: Rome", "D: Minsk"}
};

//correcte antwoorden: (0 = A, 1 = B, 2 = C, 3 = D)
byte correctAnswers[20] = {1,2,0,2,0,1,1,2,3,2,1,3,0,1,2,2,3,1,2,3};


void setup()
{
  pinMode(btnPrev, INPUT);
  pinMode(btnNext, INPUT);
  pinMode(btnSelect, INPUT);
  pinMode(btnToggle, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledLamp, OUTPUT);

  int adresID = deelnemerID + 1;
  Wire.begin(adresID); // sluit aan op I2C bus met als adres deelnemerID+1
  Wire.onReceive(ontvanger); // gebruik 'ontvanger' functie als er iets gestuurd word door de master
  Wire.onRequest(antwoorden); // gebruik 'antwoorden' functie als er iets gevraagd word door de master

  lcd.begin(16,2); // initialiseer lcd scherm
  
  attachInterrupt(0, toggleMenu, CHANGE); //interrupt voor toggle button
}


void loop()
{
  //zorg ervoor dat ledlamp uitgaat indien deze aan is geweest (buzzer ronde)
  digitalWrite(ledLamp, HIGH);
  digitalWrite(ledLamp, LOW);
  
  if (rondeNummer == 0) { //welkomst bericht voor spel start
    lcd.clear(); lcd.print("Welkom speler "); lcd.print(deelnemerID); lcd.setCursor(0, 1); lcd.print("Wacht op start..");
  }
  else if (rondeNummer > 0) { //welkomst bericht voor nieuwe ronde start
    lcd.clear(); lcd.print("Wacht op"); lcd.setCursor(0, 1); lcd.print("ronde start..");
  }
  
  //begin nieuwe ronde zodra quizmaster arduino het nieuwe ronde nummer doorstuurt
  boolean startGame = false;
  while (!startGame) {
    if (rondeNummer > huidigeRonde) {
      startGame = true;
    }
    delay(100);
  }
  huidigeRonde = huidigeRonde + 1;
  
  bezig = false; //speler kan toggle menu wel gebruiken vanaf nu
  
  //ronde info weergeven
  lcd.clear(); lcd.print("Ronde begint..");
  lcd.setCursor(0, 1); lcd.print("Ronde: "); lcd.print(rondeNummer);
  delay(2500); //geef quizmaster arduino tijd om questionIndex door te sturen.
  
  bezig = true; //speler kan toggle menu niet gebruiken vanaf nu
  
  if (questionIndex < 20) {
	gamemodeID = 1; // meerkeuze vraag
	beantwoordMeerkeuze(); //functie om antwoord te geven op de meerkeuze vraag
  } 
  
  else if (questionIndex > 19) {
	gamemodeID = 0; // buzzer vraag
	beantwoordBuzzer(); //functie om antwoord te geven op de buzzer vraag
  }
  
  //beeindig ronde en weergeef tussenstand of eindstand
  eindeRonde();
  
}



// -------- functies:

//functie voor het beantwoorden van de meerkeuze vraag:
void beantwoordMeerkeuze()
{
  printIndex = 0; //0 = antwoord A, printIndex 1 = antwoord B, enzovoorts.
  int countdown = 9; //tel seconde af vanaf 9
  int looptime = millis(); //houd de secondes bij voor countdown
  int loopstart = millis(); //starttijd van meerkeuze vraag
  int currenttime = millis(); //huidige tijd
  
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("antwoord:"); 
  lcd.setCursor(0, 1); lcd.print(antwoordenArray[questionIndex][printIndex]);
  boolean selected = false;
  while (!selected && currenttime - loopstart < 10000) { //wacht op definitieve keuze of 10 seconden
    boolean pressed = false;
    while (!pressed && currenttime - loopstart < 10000) { //wacht op button input of 10 seconden
      
	  currenttime = millis();
      if (currenttime - looptime > 1000) { //als 1 seconde verstreken is
        looptime = currenttime;
        if (countdown > 0) {
          countdown--; //tel af
        }
      }
      lcd.setCursor(14, 0); lcd.print(countdown); //print countdown rechtsboven
      
      if (digitalRead(btnPrev) == HIGH) {
        if (printIndex > 0) {
          printIndex = printIndex -1; //ga naar vorige antwoord
        }
        pressed = true;
      }
      else if (digitalRead(btnNext) == HIGH) {
        if (printIndex < 3) {
          printIndex = printIndex +1; //ga naar volgende antwoord
        }
        pressed = true;
      }
      else if (digitalRead(btnSelect) == HIGH) {
        pressed = true;
        selected = true; //maak keuze definitief
      }
      delay(50);
    }
    //weergeef het huidige antwoord:
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("antwoord:"); 
    lcd.setCursor(0, 1); lcd.print(antwoordenArray[questionIndex][printIndex]);
    delay(200); //delay voorkomt dubbele button klik
  }
  
  bezig = false; //speler kan toggle menu wel gebruiken vanaf nu
  
  //wacht minstens 16 seconden (vanaf meerkeuze start) op quizmaster
  currenttime = millis();
  while (currenttime - loopstart < 16000) {
    delay(200);
    currenttime = millis();
  }
  
  bezig = true; //speler kan toggle menu niet gebruiken vanaf nu
  
  //weergeef goed of fout
  lcd.clear(); lcd.print("Uw antwoord was:");
  if (printIndex == correctAnswers[questionIndex]) {
    lcd.setCursor(0, 1); lcd.print("goed!");
  }
  else {
    lcd.setCursor(0, 1); lcd.print("fout..");
  }
  delay(5000);
  lcd.clear();
}



//functie voor het beantwoorden van de buzzer vraag:
void beantwoordBuzzer()
{
  int responseTimeStart = millis(); //meet begin tijd buzzer vraag
  lcd.clear(); lcd.print("druk om te"); 
  lcd.setCursor(0, 1); lcd.print("antwoorden..");
  
  //wacht op button press van gebruiker
  boolean pressed = false;
  int responseTimeEnd;
  int currentBuzzerTime = millis(); //huidige tijd
  while (!pressed && currentBuzzerTime - responseTimeStart < 8000) { //wacht op button knop of 8 seconden
    currentBuzzerTime = millis();
    if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (digitalRead(btnSelect) == HIGH) || (digitalRead(btnToggle) == HIGH)) {
      pressed = true;
      responseTimeEnd = millis(); //meet eind tijd (als er gedrukt is)
    }
    else if (currentBuzzerTime - responseTimeStart >= 8000) {
      responseTimeEnd = millis(); //meet eind tijd (als er niet gedrukt is)
    }
  }
  lcd.clear(); lcd.print("Wacht op buzzer.");
  
  int reactietijdInteger = responseTimeEnd - responseTimeStart; //bereken reactietijd (integer)
  reactietijdInteger = reactietijdInteger + 1000; //zorgt dat reactietijd minimaal 4 bytes heeft (om te versturen)
  String reactietijdString = String(reactietijdInteger); //sla integer op als string
  reactietijdString.toCharArray(reactietijd,4); //sla string op als character array met lengte 4 (bv '1740')
  
  bezig = false; //speler kan toggle menu wel gebruiken vanaf nu
  
  //wacht minstens 16 seconden (vanaf buzzer start) op quizmaster
  currentBuzzerTime = millis();
  while (currentBuzzerTime - responseTimeStart < 16000) {
    delay(200);
    currentBuzzerTime = millis();
  }
  
  bezig = true; //speler kan toggle menu niet gebruiken vanaf nu
  
  lcd.clear();
}



//functie voor het ontvangen van berichten van de quizmaster arduino
void ontvanger(int numBytes)
{
  //lees eerste write (1 byte, kan waarde zijn van 1 t/m 6, geeft communicatie type aan)
  int type = Wire.read();

  if (type == 1) { // gaat om vraag nummer
	//sla de vraag index van de huidige ronde op 
	//(0 t/m 19 = meerkeuze vraag, 20 t/m 39 = buzzer vraag)
    questionIndex = Wire.read(); // receive byte as an integer
  }
  
  else if (type == 2) { // gaat om ronde info
	//sla het huidige rondenummer op
    rondeNummer = Wire.read();
  }
  
  else if (type == 3) { // gaat om ronde info
    //sla het maximaal aantal te spelen rondes op
    maxRondes = Wire.read();
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
  }
  
  else if (type == 5) { // gaat om spel status
    //spelstatus is 1 of 0 en geeft aan of het spel is afgelopen (1 = nieuwe ronde, 0 = afgelopen)
    spelStatus = Wire.read();
  }
  
  else if (type == 6) { // gaat om buzzer status
    //laat buzzer en led aangaan als quizmaster een type 6 stuurt
    tone(6, 300, 1000);
    digitalWrite(ledLamp, HIGH);
  }
}

//functie voor doorsturen van de antwoorden als quizmaster hierom vraagt.
void antwoorden()
{
  if (gamemodeID == 1) { //in geval van meerkeuze: (1 byte)
	//stuur antwoord (1 byte char)
	//printIndex 0 = antwoord A, printIndex 1 = antwoord B, enzovoorts.
    if (printIndex == 0) {Wire.write("A");}
    else if (printIndex == 1) {Wire.write("B");}
    else if (printIndex == 2) {Wire.write("C");}
    else if (printIndex == 3) {Wire.write("D");}
  }
  else if (gamemodeID == 0) { //in geval van buzzer: (4 bytes)
    Wire.write(reactietijd); //stuur reactietijd (4 byte char)
  }    
}

//toggle menu om score en ID van deelnemer te laten zien
void toggleMenu()
{
	if (!bezig) { //als het programma niet bezig is mag toggle menu gebruikt worden
		if (digitalRead(btnToggle) == HIGH) {
		  int scoreID = deelnemerID - 1; //score array van deelnemers begint op index 0
		  lcd.clear(); lcd.print("Deelnemer "); lcd.print(deelnemerID); //print deelnemerID op eerste regel
		  lcd.setCursor(0, 1); lcd.print("Score: "); lcd.print(scoreArray[scoreID]); //print score van deelnemer op tweede regel
		  lcd.print("  "); lcd.print(rondeNummer); lcd.print("/"); lcd.print(maxRondes); //print rondnummer/maxrondes op tweede regel
		}
                else if (digitalRead(btnToggle) == LOW) {
                  lcd.clear(); lcd.print("wacht.."); //wacht tot programma verder gaat
	        }
	}
	
}


//------- einde ronde:
//beeindig ronde en weergeef tussenstand of eindstand
void eindeRonde() {
  //koploper bepalen (hoogste huidige score)
  byte winnaar = 0;
  byte score;
  byte hoogsteScore = 0;
  for (byte deelnemer = 0; deelnemer < 3; deelnemer++) {
    score = scoreArray[deelnemer];
    if (score >= hoogsteScore) {
      hoogsteScore = score;
      winnaar = deelnemer + 1;
    }
  }
  
  //weergeef winnaar als spel is afgelopen
  if (spelStatus == 0) {
    rondeNummer = 0;
    lcd.clear(); lcd.print("Einde. winnaar:");
    lcd.setCursor(0, 1); lcd.print("deelnemer "); lcd.print(winnaar);
    delay(5000); //winnaar 5 seconden weergeven
  }
  
  //weergeef koploper voor volgende ronde
  else if (spelStatus == 1) {
    lcd.clear(); lcd.print("Ronde afgelopen");
    lcd.setCursor(0, 1); lcd.print("#1: speler "); lcd.print(winnaar);
    delay(5000); //koploper 5 seconden weergeven
  }
}
