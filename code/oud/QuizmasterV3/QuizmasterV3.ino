/*
	Namen: 			Delano Cörvers, Camiel Kerkhofs, Dennis Kooij, Davy Heutmekers
	Studentnummers:	1306669corvers, 1163981kooij, 1331833kerkhofs, 1309730heutmekers
	Klas:			IT1
	Module:			PIT2 (2014-2015)
	Versie:			3
	Inleverdatum:	2 februari 2015
*/

// ---------- Includes
#include <LiquidCrystal.h> // LCD Library
#include <Wire.h> // Transmission library
#include <string.h> // Library to use structs properly

// ---------- Constants
#define VERSION 3

// ---------- Variables
// LCD Screen
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LCD pins

// Buttons
int btnPrev = 5; // Previous button for the lcd menu
int btnNext = 4; // Next button for the lcd menu
int btnSelect = 3; // Select button for the lcd menu
int btnToggle = 2; // Toggle button for the score

// Menu variable
boolean browse = false;

// Identifier for the current question round
byte gameRound;
byte questionIndex;

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
byte gamemodeID; // Integer to determine gamemode
byte correctAnswer; // Buzzer question score (add or take points)

// Score array
byte scoreArray[3] = {0, 30, 40};

// getResponse variables that hold the user's answers
char multipleChoiceResponse[3]; // Save the user's input when the requested input is A, B, C or D
int responseTime[3]; // Storage the response time of each user
int roundWinner; // The index of the winner of the current round

void setup() {
	// Inputs
	pinMode(btnPrev, INPUT);
	pinMode(btnNext, INPUT);
	pinMode(btnSelect, INPUT);
	pinMode(btnToggle, INPUT);
	
	Serial.begin(9600); // Serial output
	
	// Debug welcome message
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
	
	randomSeed(analogRead(0)); // Random seed using empty analog pin
}

void loop() {	
	lcd.clear();
	
	// Welcome message
	lcd.setCursor(0, 0);
	lcd.print("Welkom Quizmaster");
	lcd.setCursor(0, 1);
	lcd.print("Druk op start");
	
	// Start the game
	boolean startGame = false;
	while (!startGame) {
		if (digitalRead(btnPrev) == HIGH) {
			qMenu(); // Get input from user, to determine the maximum amount of questions for this round
			startGame = true;
		}
	}
	
	Serial.print("Done with qMenu\n\n");
	
	lcd.clear();
	
	transmitRoundMax(maxQuestions); delay(100); // Send the maximum amount of questions to the slave arduinos
	
	// buzzerQuestion1 specification
	buzQ1.index = 0;
	strcpy(buzQ1.question, "Wat is de hoofdstad van Nederland?");
	strcpy(buzQ1.answer, "Amsterdam");
	buzQ1.correctScore = 4;
	buzQ1.incorrectScore = -2;
	
	gameRound = 1; // Init game round at 1
	while (gameRound <= maxQuestions) {
		gmMenu(); // Choose gamemode per round
		
		switch(gamemodeID) {
			case 0: bMenu(); // Buzzer question		
			//case 1:  // Multiple choice question
		}
	}
}

// Function to handle the qMenu
void displayMaxQuestions(int amount) {
	// Print the value to the screen
    lcd.clear();
    lcd.print("Aantal vragen:");
    lcd.setCursor(0, 1);
    lcd.print(amount);
	
	delay(200); // Delay to make sure the buttons do not overflow
	
	boolean select = false;
	while (!select) {
		// If current amount is selected, exit browse while; else continue to loop in browse
		if (digitalRead(btnSelect) == HIGH) {
			maxQuestions = amount; // Max amount of questions
			select = true; // Stop asking the user to enter the button to select an amount
			browse = true; // End the main menu cycle loop
		}
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH)) select = true;
	}
}

// Function to handle the bMenu
void buzzerCheck(int correctAnswer) {
	// Print the value to the screen
	lcd.clear();
	lcd.print("Antwoord");
	lcd.setCursor(0, 1);
	
	switch(correctAnswer) {
		case 0: {
			lcd.print("Incorrect");
			//scoreArray[roundWinner] -= buzQ1.incorrectScore;
			break;
		}
		case 1: {
			lcd.print("Correct");
			//Serial.println(scoreArray[roundWinner]);
			//scoreArray[roundWinner] = scoreArray[roundWinner] + buzQ1.correctScore;
			//Serial.println(scoreArray[roundWinner]);
			break;
		}
	}
	
	delay(200); // Delay to make sure the buttons do not overflow
	
	boolean select = false;
	while (!select) {
		// If current amount is selected, exit browse while; else continue to loop in browse
		if (digitalRead(btnSelect) == HIGH) {
			select = true; // Stop asking the user to enter the button to select an amount
			browse = true; // End the main menu cycle loop
		}
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH)) select = true;
	}
}

// Function to handle the gmMenu
void gamemode(int identifier) {
	// Print the value to the screen
	lcd.clear();
	lcd.print("Spel modus:");
	lcd.setCursor(0, 1);
	
	switch(identifier) {
		case 0: lcd.print("Buzzer"); break;
		case 1: lcd.print("Meerkeuze"); break;
	}
	
	delay(200); // Delay to make sure the buttons do not overflow
	
	boolean select = false;
	while (!select) {
		// If current amount is selected, exit browse while; else continue to loop in browse
		if (digitalRead(btnSelect) == HIGH) {
			gamemodeID = identifier;
			select = true; // Stop asking the user to enter the button to select an amount
			browse = true; // End the main menu cycle loop
		}
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH)) select = true;
	}
}

// Function to display the maximum amount of question menu
void qMenu() {
	int item = 0; // Integer to select what amount will be handled (Max value is 3)
	
	while (!browse) {
		if (digitalRead(btnPrev) == HIGH) {
			if (item < 1) item = 0; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item--;
			
			switch(item) {
				case 0: displayMaxQuestions(5); break;
				case 1: displayMaxQuestions(10); break;
				case 2: displayMaxQuestions(15); break;
				case 3: displayMaxQuestions(20); break;
			}
		}
		else if (digitalRead(btnNext) == HIGH) {
			if (item > 2) item = 3; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item++;
			
			switch(item) {
				case 0: displayMaxQuestions(5); break;
				case 1: displayMaxQuestions(10); break;
				case 2: displayMaxQuestions(15); break;
				case 3: displayMaxQuestions(20); break;
			}
		}
	}
	
	// Reset the browse boolean for other menus
	browse = false;
}

// Function to determine the gamemode
void gmMenu() {
	int item = 0; // Integer to select what amount will be handled (Max value is 3)
	
	while (!browse) {
		if (digitalRead(btnPrev) == HIGH) {
			if (item < 1) item = 0; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item--;
			
			switch(item) {
				case 0: gamemode(0); break;
				case 1: gamemode(1); break;
			}
		}
		else if (digitalRead(btnNext) == HIGH) {
			if (item > 2) item = 3; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item++;
			
			switch(item) {
				case 0: gamemode(0); break;
				case 1: gamemode(1); break;
			}
		}
	}
	
	// Reset the browse boolean for other menus
	browse = false;
}

// Function to display the maximum amount of question menu
void bMenu() {
	int item = 0; // Integer to select what amount will be handled (Max value is 3)
	
	while (!browse) {
		if (digitalRead(btnPrev) == HIGH) {
			if (item < 1) item = 0; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item--;
			
			switch(item) {
				case 0: buzzerCheck(0); break;
				case 1: buzzerCheck(1); break;
			}
		}
		else if (digitalRead(btnNext) == HIGH) {
			if (item > 2) item = 3; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item++;
			
			switch(item) {
				case 0: buzzerCheck(0); break;
				case 1: buzzerCheck(1); break;
			}
		}
	}
	
	// Reset the browse boolean for other menus
	browse = false;
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

// Function to execute the buzzer gamemode
void buzzerQuestion() {
	
	questionIndex = random(0, 20); // Get a random number to use as an index in the question and score array
	transmitRoundNum(gameRound); delay(100); // Transmit the round number to slave arduinos
	
	displayQuestion(); // Display the question and transmit the questionIndex
	
	// Display correct answer
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Juist antwoord:");
	lcd.setCursor(0, 1);
	lcd.print(buzQ1.answer);
	
	delay(3500);
	
	//handleResponse(0); // Request user input
	
	// Give points or take points from participant
	//fastestParticipant();
	//Serial.print(roundWinner);
	
	lcd.clear();
	
	bMenu();
}

// Function that displays the question on LCD and transmits the questionIndex to slaves
void displayQuestion() {
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
}

// Function to scroll through the screen
void lcdScroll() {
	for (int i = 0; i < 28; i++) {
		// Scroll one position left
		lcd.scrollDisplayLeft();
		delay(250);
	}
}

// Function that sends the question index to slaves
void transmitQuestion(byte questionIndex) {
  for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
    Wire.beginTransmission(device); // Transmit to device #x
    Wire.write(1); // Identifer to tell the slave arduinos that the next byte is a question
    Wire.write(questionIndex); // Send the questionIndex (1 byte)
    Wire.endTransmission(); // Stop transmitting
  }
}

// Function that sends the round number to slaves
void transmitRoundNum(byte roundNumber) {
  for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
    Wire.beginTransmission(device); // Transmit to device #x
    Wire.write(2); // Identifer to tell the slave arduinos that the next byte is the round number
    Wire.write(roundNumber);
    Wire.endTransmission();
  }
}

// Function that sends the maximum amount of rounds to slaves
void transmitRoundMax(byte roundMax) {
  for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
    Wire.beginTransmission(device); // Transmit to device #x
    Wire.write(3); // Identifer to tell the slave arduinos that the next byte is the maximum amount of questions
    Wire.write(roundMax);
    Wire.endTransmission();
  }
}

// Function that sends the score to slaves
void transmitScore(byte *scoreArray) {
	for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
	  Wire.beginTransmission(device); // Transmit to device #x
	  Wire.write(4); // Identifer to tell the slave arduinos that the next byte is the score array
	  Wire.write(scoreArray, 3);
	  Wire.endTransmission();
	}
}

// Function that sends the game state to slaves (true or false -> next round or game ended)
void transmitGameStatus(byte spelStatus) {
	for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
	  Wire.beginTransmission(device); // Transmit to device #x
	  Wire.write(5); // Identifer to tell the slave arduinos that the next byte is the game state
	  Wire.write(spelStatus);
  	  Wire.endTransmission();
	}
}

// Function that handles/requests the users input
void handleResponse(byte gamemodeID) {
    for (int device = 0; device < 1; device++) {
		switch(gamemodeID) {
			
			// Incase the gamemode is buzzer
			case 0: {
				char inputPlayer[4]; // Temporary store the quested user input in a string of 4 characters
				int time = 0; // Storage user response time as an integer
        
				switch(device) {
					case 0: Wire.requestFrom(2, 1); // Request one byte from slave arduino #2
					case 1: Wire.requestFrom(3, 1); // Request one byte from slave arduino #3
					case 2: Wire.requestFrom(4, 1); // Request one byte from slave arduino #4
				}
        
				Wire.requestFrom(2, 4); // Request four byte from slave arduino #2
				Serial.print("\nvoor: ");
				Serial.print(inputPlayer);
		
				for (int x = 0; x < 4; x++) {
					inputPlayer[x] = Wire.read(); // Storage the bytes as characters
					time = 10 * time + inputPlayer[x] - 48; // Convert string to integer (EG: "1740" -> 1740)
					Serial.print(time);
					Serial.print(" ");
				}
		
				responseTime[device] = time;
				Serial.print("\nReactietijd speler 1 char: ");
				Serial.print(inputPlayer);
				Serial.print("\nReactietijd speler 1 int: ");
				Serial.print(time);
				delay(200);
			}
			
			// Incase the gamemode is multiple choice
			case 1: {
				switch(device) {
					case 0: Wire.requestFrom(2, 1); // Request one byte from slave arduino #2
					case 1: Wire.requestFrom(3, 1); // Request one byte from slave arduino #3
					case 2: Wire.requestFrom(4, 1); // Request one byte from slave arduino #4
        
					multipleChoiceResponse[device] = Wire.read(); // Storage the bytes that the arduinos sent
					delay(200);
					Serial.print("\n\nAntwoord speler: ");
					Serial.print(multipleChoiceResponse[device]);
				}
			}
		}
    }
}

