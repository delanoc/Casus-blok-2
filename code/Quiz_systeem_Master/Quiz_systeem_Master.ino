/*
	Namen: 			Delano Cörvers, Camiel Kerkhofs, Dennis Kooij, Davy Heutmekers
	Studentnummers:	1306669corvers, 1163981kooij, 1331833kerkhofs, 1309730heutmekers
	Klas:			IT1
	Module:			PIT2 (2014-2015)
	Versie:			1.25
	Inleverdatum:	2 februari 2015
*/

// ---------- Includes
#include <LiquidCrystal.h> // LCD Library
#include <Wire.h> // Transmission library
#include <string.h>

// ---------- Constants
#define VERSION		1.25 // Based on month.day

// Buttons
#define btnPrev		5 // Previous button for the lcd menu
#define btnNext		4 // Next button for the lcd menu
#define btnSelect	3 // Select button for the lcd menu
#define btnToggle	2 // Toggle button for the score

// ---------- Variables
// LCD Screen
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LCD pins

// Menu variable
boolean browse = false;
byte first; // Identifer to automatically start a menu or return to a menu from an interrupt
byte state; // 0 = no toggle; 1 = qMenu; 2 = gmMenu; 3 = bMenu;

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
byte scoreArray[3] = {0, 0, 0};

// getResponse variables that hold the user's answers
char multipleChoiceResponse[3]; // Save the user's input when the requested input is A, B, C or D
int responseTime[3]; // Storage the response time of each user
byte participant = 1; // The index of the winner of the current round (based on the fastest buzzer response)

void setup() {
	// Inputs
	pinMode(btnPrev, INPUT);
	pinMode(btnNext, INPUT);
	pinMode(btnSelect, INPUT);
	pinMode(btnToggle, INPUT);
	
	// Begin serial, wire and lcd
	Serial.begin(9600);
	Wire.begin(); // Join I2C bus
	lcd.begin(16, 2); lcd.clear(); // 16 columns, 2 rows
	
	// Startup message
	lcd.setCursor(0, 0); // Set the cursor to the first column and the first row
	lcd.print("Quiz systeem");
	lcd.setCursor(0, 1); // Set the cursor to the first column and the second row
	lcd.print("Versie:");
	lcd.setCursor(8, 1); // Set the cursor to the ninth column and the first row
	lcd.print(VERSION);
	
	delay(2000); // Delay to keep the welcome message on the LCD screen a little bit longer
	
	randomSeed(analogRead(0)); // Random seed using empty analog pin
	
	attachInterrupt(0, toggleScore, CHANGE); // Trigger an interrupt when the toggle button switches state
}

void loop() {	
	lcd.clear();
	state = 0; // Disable toggle
	
	// Welcome message
	lcd.setCursor(0, 0);
	lcd.print("Hallo Quizmaster");
	lcd.setCursor(0, 1);
	lcd.print("Druk op start");
	
	// Start the game
	boolean startGame = false;
	while (!startGame) {
		if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (digitalRead(btnSelect) == HIGH)) {
			qMenu(); // Get input from user, to determine the maximum amount of questions for this round
			startGame = true;
		}
	}
	
	state = 0; // Disable toggle
	lcd.clear();
	transmitRoundMax(maxQuestions); delay(100); // Send the maximum amount of questions to the slave arduinos
	
	// buzzerQuestion1 specification
	buzQ1.index = 0;
	strcpy(buzQ1.question, "Wat is de hoofdstad van Nederland?");
	strcpy(buzQ1.answer, "Amsterdam");
	buzQ1.correctScore = 4;
	buzQ1.incorrectScore = -2;
	
	for (gameRound = 1; gameRound <= maxQuestions; gameRound++) {
		gmMenu(); lcd.clear(); state = 0; // Choose gamemode per round
		displayRoundNum(gameRound); delay(1750); // Display the current question relative to the total amount of questions
		
		// Load the requested gamemode
		switch(gamemodeID) {
			case 0: buzzerQuestion(); // Buzzer question		
			//case 1:  // Multiple choice question
		}
		
		// Tell participants if there is another question in the game or not
		if (gameRound != maxQuestions) transmitGameState(1); // There is a question left
		else transmitGameState(0); // There are no more questions
	}
	
	// Show the quizmaster, which participant had the highest score and won
	//gameWinner(); // Get the highest score in the scoreArray
	
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Spel winnaar:");
	lcd.setCursor(0, 1);
	lcd.print(gameWinner());
	
	delay(5000); // The game has ended, keep winner on screen for 5 seconds
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
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (first == 1)) select = true;
	}
}

// Function to handle the bMenu
void buzzerCheck(int correctAnswer) {
	// Print the value to the screen
	lcd.clear();
	lcd.print("Antwoord:");
	lcd.setCursor(0, 1);
	
	switch(correctAnswer) {
		case 0: lcd.print("Onjuist"); break;
		case 1: lcd.print("Juist"); break;
	}
	
	delay(200); // Delay to make sure the buttons do not overflow
	
	boolean select = false;
	while (!select) {
		// If current amount is selected, exit browse while; else continue to loop in browse
		if (digitalRead(btnSelect) == HIGH) {
			switch(correctAnswer) {
				case 0: {
					scoreArray[participant] += buzQ1.incorrectScore;
					break;
				}
				case 1: {
					scoreArray[participant] += buzQ1.correctScore;
					break;
				}
			}	
			select = true; // Stop asking the user to enter the button to select an amount
			browse = true; // End the main menu cycle loop
		}
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (first == 1)) select = true;
	}
}

// Function to handle the gmMenu
void gamemode(int identifier) {
	// Print the value to the screen
	lcd.clear();
	lcd.print("Kies spel modus:");
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
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (first == 1)) select = true;
	}
}

// Function to display the maximum amount of question menu
void qMenu() {
	state = 1; // Set the current state to 1
	byte item = 0; // Byte to select what amount will be handled (Max value is 3)
	first = 1; // Byte to determine if the menu is going through its first run (this is used to make sure the menu opens, without pressing a button first)
	
	while (!browse) {
		// If the previous button is pressed or it is the first run through
		if ((digitalRead(btnPrev) == HIGH) || (first == 1)) {
			first = 0; // First run through has ended
			if (item < 1) item = 0; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item--;
			
			switch(item) {
				case 0: displayMaxQuestions(5); break;
				case 1: displayMaxQuestions(10); break;
				case 2: displayMaxQuestions(15); break;
			}
		}
		else if (digitalRead(btnNext) == HIGH) {
			if (item > 2) item = 3; // Incase item is larger than 3 (there are 4 amount choices), reset the counter to 0 (going back to the first amount chocie)
			else item++;
			
			switch(item) {
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
	state = 2; // Set the toggle state to 2
	first = 1; // Byte to determine if the menu is going through its first run (this is used to make sure the menu opens, without pressing a button first)
	
	while (!browse) {
		// If the previous button is pressed or it is the first run through
		if ((digitalRead(btnPrev) == HIGH) || (first == 1)) {
			first = 0; // First run through has ended
			gamemode(0);
		}
		else if (digitalRead(btnNext) == HIGH) gamemode(1);
	}
	
	// Reset the browse boolean for other menus
	browse = false;
}

// Function to display the maximum amount of question menu
void bMenu() {
	state = 3; // Set the toggle state to 3
	byte item = 0; // Byte to select what amount will be handled (Max value is 3)
	first = 1; // Byte to determine if the menu is going through its first run (this is used to make sure the menu opens, without pressing a button first)
	
	while (!browse) {
		// If the previous button is pressed or it is the first run through
		if ((digitalRead(btnPrev) == HIGH) || (first == 1)) {
			first = 0; // First run through has ended
			buzzerCheck(0);
		}
		else if (digitalRead(btnNext) == HIGH) buzzerCheck(1);
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
	
	//handleResponse(0); // Request user input for buzzer gamemode
	
	bMenu();
	lcd.clear();
}

// Function to get the round winner based on the highest score
byte gameWinner() {
	byte array[3];
	for (int i = 0; i < 3; i++) { 
		array[i] = scoreArray[i]; // Add index value of mainArray into a tempArray in the same index
	}
	
	byte max = 0;
	for (int i = 0; i < 3; i++) {
		if (array[i] > max) max = array[i]; // If array index is larger than 0, max gets that value
	}
	
	for (int i = 0; i < 3; i++) {
		if (scoreArray[i] == max) return i + 1; // If any number in the score array is equal to max, return that participant + 1 -> Index starts at 0, playerids at 1
	}
}

// Function to get the lowest response time
void fastestResponse() {
	byte array[3];
	for (int i = 0; i < 3; i++) { 
		array[i] = responseTime[i]; // Add index value of mainArray into a tempArray in the same index
	}
	
	byte min = 11;
	for (int i = 0; i < 3; i++) {
		if (array[i] < min) min = array[i]; // If array index is smaller than 11, min gets that value
	}
	
	for (int i = 0; i < 3; i++) {
		if (responseTime[i] == min) participant = i; // If any number in the score array is equal to min, assign that number to participant
	}
}

// Function that displays the question on LCD and transmits the questionIndex to slaves
void displayQuestion() {
	lcd.clear();
	lcd.setCursor(2, 0);
	lcd.print(buzQ1.question);
	
	transmitQuestion(questionIndex); delay(100); // Send the questionIndex to the slaves (so they know what the response options are)
	// START INTERRUPT FOR GET ANSWERS
	
	for (int i = 0; i < 64; i++) {
		// Scroll one position left
		lcd.scrollDisplayLeft();
		delay(300);
	}
}

// Function that toggles the LCD display to show the score of each player
void toggleScore() {
	if (digitalRead(btnToggle) == HIGH) {
		if ((state == 1) || (state == 2) || (state == 3)) {
			lcd.clear();
			// Row 1 -> playerid
			lcd.setCursor(0, 0);
			lcd.print("D:");
			lcd.setCursor(4, 0);
			lcd.print("1");
			lcd.setCursor(8, 0);
			lcd.print("2");
			lcd.setCursor(12, 0);
			lcd.print("3");
			// Row 2 -> score points
			lcd.setCursor(0, 1);
			lcd.print("S:");
			lcd.setCursor(4, 1);
			lcd.print(scoreArray[0]);
			lcd.setCursor(8, 1);
			lcd.print(scoreArray[1]);
			lcd.setCursor(12, 1);
			lcd.print(scoreArray[2]);
		}
	}
	else if (digitalRead(btnToggle) == LOW) {
		switch(state) {
			case 1: lcd.clear(); first = 1; break; // qMenu()
			case 2: lcd.clear(); first = 1; break; // gmMenu()
			case 3: lcd.clear(); first = 1; break; // bMenu()
		}
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
void transmitGameState(byte state) {
	for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
	  Wire.beginTransmission(device); // Transmit to device #x
	  Wire.write(5); // Identifer to tell the slave arduinos that the next byte is the game state
	  Wire.write(state);
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
				
				// What does this do?
				Wire.requestFrom(2, 4); // Request four bytes from slave arduino #2 (the time)
				Serial.print("\nvoor: ");
				Serial.print(inputPlayer);
		
				for (int x = 0; x < 4; x++) {
					inputPlayer[x] = Wire.read(); // Store the bytes as characters
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

