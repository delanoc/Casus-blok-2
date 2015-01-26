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
#include <string.h> // Library to use structs properly

// ---------- Constants
#define VERSION 2

// ---------- Variables
// LCD Screen
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LCD pins

// Buttons
int btnPrev = 5; // Previous button for the lcd menu
int btnNext = 4; // Next button for the lcd menu
int btnSelect = 3; // Select button for the lcd menu
int btnToggle = 2; // Toggle button for the score

/*
// LEDs
int ledGreen = 13; // Green LED for when the user has given the correct answer

// Buzzer
int buzzer = 7;
*/


// Struct for buzzer questions
struct buzzerQuestions {
	int index;
	char question[80];
	char answer[24];
	byte correctScore;
	byte incorrectScore;
};
struct buzzerQuestions buzQ1;

// Game and round variables
byte maxQuestions; // Maximum amount of questions
int gamemodeID; // Integer to determine gamemode
int correctAnswer; // Buzzer question score (add or take points)

// Score array
byte scoreArray[3] = {0, 30, 40};

// getResponse variables that hold the user's answers
char multipleChoiceResponse[3]; // sla het antwoord van de speler op als een char (deze is "A", "B", "C" of "D")
int responseTime[3]; // sla de reactietijden op in een array als integers
int roundWinner;

void setup() {
	// Inputs
	pinMode(btnPrev, INPUT);
	pinMode(btnNext, INPUT);
	pinMode(btnSelect, INPUT);
	pinMode(btnToggle, INPUT);

	randomSeed(analogRead(0)); // Random seed using empty analog pin
	
	Serial.begin(9600); // Serial output
	Serial.print("Quizmaster system by Delano Corvers, Camiel Kerkhofs, Dennis Kooij en Davy Heutmekers");
	Serial.print("\nVersion: ");
	Serial.print(VERSION);
	Serial.print("\n____________________________________________\n\n"); // Division line for serial output
	
	Wire.begin(); // join i2c bus (address optional for master)
	
	lcd.begin(16, 2); // Start the LCD display with 16 columns and 2 rows
	lcd.clear();
	
	lcd.setCursor(0, 0); // Set the cursor to the first column and the first row
	lcd.print("Quizmaster game");
	lcd.setCursor(0, 1); // Set the cursor to the first column and the second row
	lcd.print("Version:");
	lcd.setCursor(9, 1); // Set the cursor to the ninth column and the first row
	lcd.print(VERSION);
	
	delay(2000); // Delay to keep the welcome message on the LCD screen a little bit longer
	
	lcd.clear();
}

void loop() {	
	lcd.clear();
	
	// Welcome message
	lcd.setCursor(0, 0);
	lcd.print("Welkom");
	lcd.setCursor(0, 1);
	lcd.print("Druk op start");
	
	// While loop to start the game
	boolean startGame = false;
	while (!startGame) {
		if (digitalRead(btnToggle) == HIGH) {
			questionsMenu(); // Get input from user, to determine the maximum amount of questions for this round
			startGame = true;
		}
	}
	
	lcd.clear();
	
	// Send the maximum amount of questions to the slaves
	transmitRoundMax(maxQuestions); delay(100);
	
	// buzzerQuestion 1 specification
	buzQ1.index = 0;
	strcpy(buzQ1.question, "Wat is de hoofdstad van Nederland?");
	strcpy(buzQ1.answer, "Amsterdam");
	buzQ1.correctScore = 4;
	buzQ1.incorrectScore = -2;
	
	// Game round starts
	byte gameRound = 1; // Init game round at 1
	while (gameRound <= maxQuestions) {
		gamemodeMenu(); // Open the menu to choose the gamemode for one specific round
		
		// If gamemode is buzzer
		if (gamemodeID == 0) {
			
			byte questionIndex = random(0, 20); // Get a random number to use as an index in the question and score array
			
			transmitRoundNum(gameRound); delay(100);
			
			// Display question
			lcd.clear();
			// First row
			displayRoundNum(gameRound); // Display question in first row
			delay(1000);
			
			// Second row
			lcd.clear();
			lcd.setCursor(5, 0);
			lcd.print(buzQ1.question);			
			
			transmitQuestion(questionIndex); delay(100); // Send the questionIndex to the slaves (so they know what the response options are)
			
			lcdScroll(); // Scroll through the question
			
			// Display correct answer
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Antwoord model:");
			lcd.setCursor(0, 1);
			lcd.print(buzQ1.answer);
			
			delay(8000); // Wait for user response
			
			// Request user input
			getResponse(0); // Gamemode is buzzer
			
			// Give points or take points from participant
			fastestParticipant();
			Serial.print(roundWinner);
			
			
			lcd.clear();
			buzzerMenu();
			
			for (int i = 0; i < 3; i++) {
				Serial.print(scoreArray[i]);
			}
			
			gameRound++;
			
			
			/* ---------------------------------------------------
				SEND QUESTIONINDEX TO SLAVES
				REQUEST USER INPUT
				GET USE INPUT
			 --------------------------------------------------- */
			
			
			transmitScore(scoreArray); delay(100);
			//transmitGameStatus(spelStatus); delay(100);
			
			
			/*
			// If the quizmaster wants to quit the game before all questions have been asked, check for button
			while (!quit) {
				if (digitalRead(btnSelect) == HIGH) {
					
				}
			}
			*/
		}
		// If gamemode is multiple choice
		else if (gamemodeID == 1) {
			
			int questionIndex = random(20, 40); // Stuur naar andere slaves
			
		}
		
		gameRound++; // Proceed to next round
	}
}

// Function to display the question number relative to the max questions
void displayRoundNum(int currentQuestion) {
	// First row displays the current question
	lcd.setCursor(0, 0);
	lcd.print("Vraag:");
	lcd.setCursor(7, 0);
	lcd.print(currentQuestion);
	lcd.setCursor(9, 0);
	lcd.print("//");
	lcd.setCursor(10, 0);
	lcd.print(maxQuestions);
}

// Function to scroll through the screen
void lcdScroll() {
	for (int i = 0; i < 26; i++) {
		// Scroll one position left
		lcd.scrollDisplayLeft();
		delay(250);
	}
}

// Function that prompts the quizmaster to choose the amount of questions
void questionsMenu() {
    boolean selected = false;
    
	maxQuestions = 5; // The minimum amount of questions
	
    while (!selected) {
		
        boolean pressed = false;
        while (!pressed) {
			delay(250);
            if (digitalRead(btnPrev) == HIGH) {
                if (maxQuestions >= 10) maxQuestions -= 5; // The user cannot choose less than five questions
                pressed = true;
            }
            else if (digitalRead(btnNext) == HIGH) {
                if (maxQuestions <= 15) maxQuestions += 5; // The user cannot choose more than twenty question
                pressed = true;
            }
            else if (digitalRead(btnSelect) == HIGH) {
				// End both while loops
				pressed = true;
				selected = true;
			}
			
			// Print the value to the screen
            lcd.clear();
            lcd.print("Aantal vragen:");
            lcd.setCursor(0, 1);
            lcd.print(maxQuestions);
        }
    }
}

// Function that prompts the quizmaster to choose the gamemode
void gamemodeMenu() {
    boolean selected = false;
    
	gamemodeID = 0;
	
    while (!selected) {
		
        boolean pressed = false;
        while (!pressed) {			
            if (digitalRead(btnPrev) == HIGH) {
                gamemodeID = 0;
                pressed = true;
            }
            else if (digitalRead(btnNext) == HIGH) {
                gamemodeID = 1;
                pressed = true;
            }
            else if (digitalRead(btnSelect) == HIGH) {
				// End both while loops
				pressed = true;
				selected = true;
			}
			
			// Print the value to the screen
            lcd.clear();
            lcd.print("Spel modus:");
            lcd.setCursor(0, 1);
			
			if (gamemodeID == 0) lcd.print("Buzzer");
			else if (gamemodeID == 1) lcd.print("Meerkeuze");
			
			// Prevent overflow buttons
            delay(250);
        }
    }
}

// Function that prompts the quizmaster to give or take score points
void buzzerMenu() {
    boolean selected = false;
    
	correctAnswer = 0;
	
    while (!selected) {
		
        boolean pressed = false;
        while (!pressed) {			
            if (digitalRead(btnPrev) == HIGH) {
                correctAnswer = 0;
                pressed = true;
            }
            else if (digitalRead(btnNext) == HIGH) {
                correctAnswer = 1;
                pressed = true;
            }
            else if (digitalRead(btnSelect) == HIGH) {
				// End both while loops
				pressed = true;
				selected = true;
			}
			
			// Prevent overflow buttons
            delay(250);
        }
    }
	
	// Print the value to the screen
	lcd.clear();
	lcd.print("Goed antwoord?");
	lcd.setCursor(0, 1);
	
	if (correctAnswer == 0) {
		lcd.print("Incorrect");
		scoreArray[roundWinner] -= buzQ1.incorrectScore;
	}
	else if (correctAnswer == 1) {
		lcd.print("Correct");
		Serial.println(scoreArray[roundWinner]);
		scoreArray[roundWinner] = scoreArray[roundWinner] + buzQ1.correctScore;
		Serial.println(scoreArray[roundWinner]);
	}
}

// Function to get the fastest participant
void fastestParticipant() {
	int fastestTime = 10000;
	for (int i = 0; i < 3; i++) {
		if (scoreArray[i] < fastestTime) {
			roundWinner = i;
			fastestTime = scoreArray[i];
			// buzzer response
		}
		scoreArray[roundWinner] = 10000;
	}
}

// Function that sends the question to slaves
void transmitQuestion(byte questionIndex) { //stuurt vraag nummer naar slaves
  for (byte deviceID = 2; deviceID < 5; deviceID++) {
    Wire.beginTransmission(deviceID); // transmit to device #x
    Wire.write(1); //geeft aan dat het om de vraag gaat (1)
    Wire.write(questionIndex); // send 1 bytes (vraag nummer)
    Wire.endTransmission(); // stop transmitting
  }
}

// Function that sends the round number to slaves
void transmitRoundNum(byte roundNumber) {
  for (byte deviceID = 2; deviceID < 5; deviceID++) { //stuur het volgende aan device 2, 3 en 4
    Wire.beginTransmission(deviceID); //stuur het volgende aan device 2, 3 en 4
    Wire.write(2);
    Wire.write(roundNumber);
    Wire.endTransmission();
  }
}

// Function that sends the maximum amount of rounds to slaves
void transmitRoundMax(byte roundMax) {
  for (byte deviceID = 2; deviceID < 5; deviceID++) {
    Wire.beginTransmission(deviceID);
    Wire.write(3);
    Wire.write(roundMax);
    Wire.endTransmission();
  }
}

// Function that sends the score to slaves
void transmitScore(byte *scoreArray) { //stuurt score array naar slaves
	for (byte deviceID = 2; deviceID < 5; deviceID++) { //stuur het volgende aan device 2, 3 en 4
	  Wire.beginTransmission(deviceID); // transmit to device #x
	  Wire.write(4); //geeft aan dat het om de score gaat (4)
	  Wire.write(scoreArray, 3); // send score array (3 bytes)
	  Wire.endTransmission(); // stop transmitting
	}
}

// Function that sends the game state to slaves
void transmitGameStatus(byte spelStatus) { //stuurt spel status naar slaves (true of false, volgende ronde of spel afgelopen?)
	for (byte deviceID = 2; deviceID < 5; deviceID++) { //stuur het volgende aan device 2, 3 en 4
	  Wire.beginTransmission(deviceID); // transmit to device #x
	  Wire.write(5); //geeft aan dat het om de spelstatus gaat (5)
	  Wire.write(spelStatus); // send 1 bytes (0 of 1)
  	  Wire.endTransmission(); // stop transmitting
	}
}

// Function that handles the users input
void getResponse(int gamemodeID) { //vraagt slaves om antwoord op vraag
    //gamemodeID, 1= meerkeuze, 0=buzzer
    
    for (int y = 0; y < 1; y++) {
      
      if (gamemodeID == 1) { //in geval van meerkeuze:
        
        if (y == 0) { Wire.requestFrom(2, 1); }    // request 1 bytes from slave device #2
        else if (y == 1) { Wire.requestFrom(3, 1); }    // request 1 bytes from slave device #2
        else if (y == 2) { Wire.requestFrom(4, 1); }    // request 1 bytes from slave device #2
        
        multipleChoiceResponse[y] = Wire.read(); //sla de byte op
        delay(200);
        Serial.print("\n\n Antwoord speler: "); Serial.print(multipleChoiceResponse[y]);
      }
      
      else if (gamemodeID == 0) { //in geval van buzzer:
        char inputPlayer[4]; // sla het onvangen bericht tijdelijk op als een string van 4 characters
        int time = 0; // sla reactietijd op als integer
        
        if (y == 0) { Wire.requestFrom(2, 1); }    // request 1 bytes from slave device #2
        else if (y == 1) { Wire.requestFrom(3, 1); }    // request 1 bytes from slave device #2
        else if (y == 2) { Wire.requestFrom(4, 1); }    // request 1 bytes from slave device #2
        
        Wire.requestFrom(2, 4);    // request 4 bytes from slave device #2
        Serial.print("\n voor: "); Serial.print(inputPlayer);
        for (int x = 0; x < 4; x++) {
          inputPlayer[x] = Wire.read(); //sla de bytes op als characters
          time = 10 * time + inputPlayer[x] - 48; //reken de string om naar een integer (bv: string "1740" wordt een integer met waarde 1740)
          Serial.print(time); Serial.print(" ");
          //Serial.print("\n read: "); Serial.print(inputPlayer[x]);
        }
        responseTime[y] = time;
        Serial.print("\n Reactietijd speler 1 char: "); Serial.print(inputPlayer);
        Serial.print("\n Reactietijd speler 1 int: "); Serial.print(time);//Serial.print(reactietijdSpeler1);
        delay(200);
      }
      
    }
    
}

