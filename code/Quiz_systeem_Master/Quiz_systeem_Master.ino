/*
	Namen: 			Delano Cörvers, Camiel Kerkhofs, Dennis Kooij, Davy Heutmekers
	Studentnummers:	1306669corvers, 1163981kooij, 1331833kerkhofs, 1309730heutmekers
	Klas:			IT1
	Module:			PIT2 (2014-2015)
	Versie:			1.27
	Inleverdatum:	2 februari 2015
*/

// ---------- Includes
#include <LiquidCrystal.h> // LCD Library
#include <Wire.h> // Transmission library
#include <string.h>
#include <avr/pgmspace.h> // Progmem library

// ---------- Constants
#define VERSION		1.27 // Based on month.day

// Buttons
#define btnPrev		5 // Previous button for the lcd menu
#define btnNext		4 // Next button for the lcd menu
#define btnSelect	3 // Select button for the lcd menu
#define btnToggle	2 // Toggle button for the score

// Misc
#define MAX_PARTICIPANTS 3

// Questions saved in PROGMEM
// Buzzer
prog_char question1[] PROGMEM = "Hoe heet een bril zonder glazen?";
prog_char question2[] PROGMEM = "Wat betekent de afkorting GUI?";
prog_char question3[] PROGMEM = "Waarvoor staat de afkorting SQL?";
prog_char question4[] PROGMEM = "Met welke techniek wint men direct bij judo?";
prog_char question5[] PROGMEM = "Welke Britse zangeres had een hit in 2006 met `Rehab’?";
prog_char question6[] PROGMEM = "Als u een `chalkie` in Australië was, wat zou uw beroep zijn?";
prog_char question7[] PROGMEM = "Wat is de eerste letter  van het Griekse alfabet?";
prog_char question8[] PROGMEM = "Welke kleur bevindt zich aan de top van een regenboog?";
prog_char question9[] PROGMEM = "Wat zijn de drie primaire kleuren van licht?";
prog_char question10[] PROGMEM = "Welke organisatie is ook gekend als ‘De cosa nostra’?";
prog_char question11[] PROGMEM = "Wat is de grootste zoekmachine op internet?";
prog_char question12[] PROGMEM = "Welke kleur komt in vlaggen over de hele wereld het meeste  voor?";
prog_char question13[] PROGMEM = "Uit welk land komt Tequila?";
prog_char question14[] PROGMEM = "Wie was de oppergod in de Griekse mythologie?";
prog_char question15[] PROGMEM = "Wat was Rocky`s achternaam in de boksfilm ‘Rocky’?";
prog_char question16[] PROGMEM = "Wat is kleiner dan een molecuul?";
prog_char question17[] PROGMEM = "Hoeveel magen heeft een vogel?";
prog_char question18[] PROGMEM = "Hoeveel ogen heeft een honingbij?";
prog_char question19[] PROGMEM = "Wat is het hoogste gras in de wereld?";
prog_char question20[] PROGMEM = "Wat is het grootste landzoogdier in de wereld?";
// Multiple choice
prog_char question21[] PROGMEM = "Wat is de hoofdstad van Nederland?";
prog_char question22[] PROGMEM = "Hoeveel vestigingen heeft Zuyd Hogeschool?";
prog_char question23[] PROGMEM = "Wat is de naam van de volgende Koning(in) van Nederland?";
prog_char question24[] PROGMEM = "Hoe heette de man van prinses Beatrix?";
prog_char question25[] PROGMEM = "Heeft WhatsApp Facebook overgekocht?";
prog_char question26[] PROGMEM = "Mag men met een autorijbewijs ook een scooter besturen?";
prog_char question27[] PROGMEM = "Welk bedrijf gebruikte als eerste een GUI?";
prog_char question28[] PROGMEM = "Welke acteur speelt de rol van Harry Potter?";
prog_char question29[] PROGMEM = "Bij welke sport spelen zowel mannen als vrouwen in één team?";
prog_char question30[] PROGMEM = "In welk jaar vond de laatste Elfstedentoch plaats?";
prog_char question31[] PROGMEM = "Hoeveel kegels moet men omwerpen bij bowlen?";
prog_char question32[] PROGMEM = "Hoeveel bollen telt het Atomium?";
prog_char question33[] PROGMEM = "Hoeveel rozijnen zitten in twee dozijnen?";
prog_char question34[] PROGMEM = "Hoeveel magen heeft een koe?";
prog_char question35[] PROGMEM = "Welke kleur had de originele Cola?";
prog_char question36[] PROGMEM = "Uit hoeveel hokjes bestaat een sudokuspel?";
prog_char question37[] PROGMEM = "Met welk toestel meet men aardbevingen?";
prog_char question38[] PROGMEM = "Wie speelt de hoofdrol in 'The Hunger Games'?";
prog_char question39[] PROGMEM = "Voor wat is Phasmaphobia de vrees?";
prog_char question40[] PROGMEM = "Volgens een oud gezegde leiden alle wegen tot een hoofdstad, welke?";

// Array pointer to flash stored question database
PROGMEM const char* questions[40] = {   
  question1, question2, question3, question4, question5, question6, question7, question8, question9, question10,
  question11, question12, question13, question14, question15, question16, question17, question18, question19, question20,
  question21, question22, question23, question24, question25, question26, question27, question28, question29, question30,
  question31, question32, question33, question34, question35, question36, question37, question38, question39, question40
};

// Correct answers array for all questions
const char* answers[40] = {"Montuur", "Graphical User Interface", "Structured Query Language", "Ippon", "Amy Winehouse", "Leerkracht", "Alpha", "Rood",
	"Rood, Blauw en Groen", "Maffia", "Google", "Rood", "Mexico", "Zeus", "Balboa", "Atoom", "2", "5", "Bamboe", "Olifant", "B", "C", "A", "C", "A",
	"A", "B", "C", "D", "C", "B", "D", "A", "B", "C", "C", "D", "B", "C", "D"};

// Scores for the questions
const byte scores[20] = {2, 8, 2, 4, 4, 4, 8, 2, 4, 8, 4, 8, 2, 2, 8, 4, 4, 2, 8, 4}; // NEED 20 MORE SCORES

// Buffer for reading promem to ram
char buffer[70];

// ---------- Variables
// LCD Screen
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LCD pins

// Menu variable
boolean browse = false;
boolean first; // Identifer to automatically start a menu or return to a menu from an interrupt
byte state; // 0 = no toggle; 1 = qMenu; 2 = gmMenu; 3 = bMenu;

// Identifier for the current question round
byte gameRound;
byte questionIndex;

// Game and round variables
byte maxQuestions; // Maximum amount of questions
byte gamemodeID; // Integer to determine gamemode
byte correctAnswer; // Buzzer question score (add or take points)

// Score array
byte scoreArray[3] = {0, 0, 0};

// getResponse variables that hold the user's answers
char multipleChoiceResponse[3]; // Save the user's input when the requested input is A, B, C or D
int responseTime[3]; // Storage the response time of each user
byte participant; // The index of the winner of the current round (based on the fastest buzzer response)

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
	lcd.setCursor(2, 0); // Set the cursor to the third column and the first row
	lcd.print("Quiz systeem");
	lcd.setCursor(2, 1); // Set the cursor to the first column and the second row
	lcd.print("Versie:");
	lcd.setCursor(10, 1); // Set the cursor to the ninth column and the first row
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
	
	Serial.println("DEBUG: The game has started");
	
	state = 0; // Disable toggle
	lcd.clear();
	transmitRoundMax(maxQuestions); delay(100); // Send the maximum amount of questions to the slave arduinos
	
	for (gameRound = 1; gameRound <= maxQuestions; gameRound++) {
		gmMenu(); lcd.clear(); state = 0; // Choose gamemode per round
		transmitRoundNum(gameRound); delay(100); // Transmit the round number to slave arduinos
		displayRoundNum(gameRound); delay(1750); // Display the current question relative to the total amount of questions
		
		// Load the requested gamemode
		switch(gamemodeID) {
			case 0: buzzerQuestion(); break; // Buzzer question		
			case 1: multipleChoiceQuestion(); break; // Multiple choice question
		}
		
		// Tell participants if there is another question in the game or not
		if (gameRound != maxQuestions) transmitGameState(1); // There is a question left
		else transmitGameState(0); // There are no more questions
	}
	
	// Show the quizmaster, which participant had the highest score and won	
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
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (first == true)) select = true;
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
					if (scoreArray[participant] == 0) scoreArray[participant] = 0;
					else scoreArray[participant] -= 2;
					break;
				}
				case 1: {
					scoreArray[participant] += scores[questionIndex];
					break;
				}
			}	
			select = true; // Stop asking the user to enter the button to select an amount
			browse = true; // End the main menu cycle loop
		}
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (first == true)) select = true;
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
		else if ((digitalRead(btnPrev) == HIGH) || (digitalRead(btnNext) == HIGH) || (first == true)) select = true;
	}
}

// Function to display the maximum amount of question menu
void qMenu() {
	state = 1; // Set the current state to 1
	byte item = 0; // Byte to select what amount will be handled (Max value is 3)
	first = true; // Byte to determine if the menu is going through its first run (this is used to make sure the menu opens, without pressing a button first)
	
	while (!browse) {
		// If the previous button is pressed or it is the first run through
		if ((digitalRead(btnPrev) == HIGH) || (first == true)) {
			first = false; // First run through has ended
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
	first = true; // Byte to determine if the menu is going through its first run (this is used to make sure the menu opens, without pressing a button first)
	
	while (!browse) {
		// If the previous button is pressed or it is the first run through
		if ((digitalRead(btnPrev) == HIGH) || (first == true)) {
			first = false; // First run through has ended
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
	first = true; // Byte to determine if the menu is going through its first run (this is used to make sure the menu opens, without pressing a button first)
	
	while (!browse) {
		// If the previous button is pressed or it is the first run through
		if ((digitalRead(btnPrev) == HIGH) || (first == true)) {
			first = false; // First run through has ended
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
	Serial.println("DEBUG: Buzzer gamemode has been selected");
	
	questionIndex = random(0, 19); // Get a random number to use as an index in the question and score array
	strcpy_P(buffer, (char*)pgm_read_word(&(questions[questionIndex])));
	
	//questionIndex = 0; // TESTING PURPOSES
	
	displayQuestion(buffer); // Display the question and transmit the questionIndex
	
	// Display correct answer
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Juist antwoord:");
	lcd.setCursor(0, 1);
	lcd.print(answers[questionIndex]);
	
	delay(3500);
	
	handleResponse(0); // Request user input for buzzer gamemode
	
	bMenu();
	displayScores(); // Print the scores for each player
	delay(1500);
	lcd.clear();
}

// Function to execute the multiple choice gamemode
void multipleChoiceQuestion() {
	Serial.println("DEBUG: Multiple choice gamemode has been selected");
	
	questionIndex = random(20, 39); // Get a random number to use as an index in the question and score array
	strcpy_P(buffer, (char*)pgm_read_word(&(questions[questionIndex])));
	
	//questionIndex = 0; // TESTING PURPOSES
	
	displayQuestion(buffer); // Display the question and transmit the questionIndex
	
	handleResponse(1); // Request user input for buzzer gamemode
	
	// Give points to all players with correct answers
	for (int i = 0; i < MAX_PARTICIPANTS; i++) {
		if (multipleChoiceResponse[i] == answers[questionIndex][0]) {
			scoreArray[i] += scores[questionIndex];
		}
	}
	
	displayScores(); // Print the scores for each player
	delay(1500);
	lcd.clear();
}

// Function to get the round winner based on the highest score
byte gameWinner() {
	byte array[3];
	for (int i = 0; i < MAX_PARTICIPANTS; i++) { 
		array[i] = scoreArray[i]; // Add index value of mainArray into a tempArray in the same index
	}
	
	byte max = 0;
	for (int i = 0; i < MAX_PARTICIPANTS; i++) {
		if (array[i] > max) max = array[i]; // If array index is larger than 0, max gets that value
	}
	
	for (int i = 0; i < MAX_PARTICIPANTS; i++) {
		if (scoreArray[i] == max) return i + 1; Serial.println("DEBUG: Game winner has been determined");// If any number in the score array is equal to max, return that participant + 1 -> Index starts at 0, playerids at 1
	}
}

// Function to get the lowest response time
void fastestResponse() {
	byte array[3];
	for (int i = 0; i < MAX_PARTICIPANTS; i++) { 
		array[i] = responseTime[i]; // Add index value of mainArray into a tempArray in the same index
	}
	
	byte min = 11;
	for (int i = 0; i < MAX_PARTICIPANTS; i++) {
		if (array[i] < min) min = array[i]; // If array index is smaller than 11, min gets that value
	}
	
	for (int i = 0; i < MAX_PARTICIPANTS; i++) {
		if (responseTime[i] == min) {
			participant = i; // If any number in the score array is equal to min, assign that number to participant
			transmitBuzzerState();
			
			Serial.println("DEBUG: Fastest response has been detected.");
		}
	}
}

// Function that displays the question on LCD and transmits the questionIndex to slaves
void displayQuestion(String question) {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(question);
	
	Serial.print("Question is being displayed on the LCD");
	
	transmitQuestion(questionIndex); delay(100); // Send the questionIndex to the slaves (so they know what the response options are)
	
	for (int i = 0; i < 32; i++) {
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
			displayScores(); // Print the scores to the screen
			
			Serial.println("DEBUG: Quizmaster toggled score");
		}
	}
	else if (digitalRead(btnToggle) == LOW) {
		if ((state == 1) || (state == 2) || (state == 3)) lcd.clear(); first = true;
	}
}

// Function to print the scores of each user to the screen
void displayScores() {
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

// Function that sends the question index to slaves
void transmitQuestion(byte questionIndex) {
  for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
    Wire.beginTransmission(device); // Transmit to device #x
    Wire.write(1); // Identifer to tell the slave arduinos that the next byte is a question
    Wire.write(questionIndex); // Send the questionIndex (1 byte)
    Wire.endTransmission(); // Stop transmitting
	
	Serial.println("DEBUG: Question has been transmitted to a slave");
  }
}

// Function that sends the round number to slaves
void transmitRoundNum(byte roundNumber) {
  for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
    Wire.beginTransmission(device); // Transmit to device #x
    Wire.write(2); // Identifer to tell the slave arduinos that the next byte is the round number
    Wire.write(roundNumber);
    Wire.endTransmission();
	
	Serial.println("DEBUG: Round number has been transmitted to a slave");
  }
}

// Function that sends the maximum amount of rounds to slaves
void transmitRoundMax(byte roundMax) {
  for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
    Wire.beginTransmission(device); // Transmit to device #x
    Wire.write(3); // Identifer to tell the slave arduinos that the next byte is the maximum amount of questions
    Wire.write(roundMax);
    Wire.endTransmission();
	
	Serial.println("DEBUG: Max amount of questions has been transmitted to a slave");
  }
}

// Function that sends the score to slaves
void transmitScore(byte *scoreArray) {
	for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
	  Wire.beginTransmission(device); // Transmit to device #x
	  Wire.write(4); // Identifer to tell the slave arduinos that the next byte is the score array
	  Wire.write(scoreArray, 3);
	  Wire.endTransmission();
	  
	  Serial.println("DEBUG: Scores have been transmitted to a slave");
	}
}

// Function that sends the game state to slaves (true or false -> next round or game ended)
void transmitGameState(byte state) {
	for (byte device = 2; device < 5; device++) { // Send the round number to device 2, 3 and 4 one by one
	  Wire.beginTransmission(device); // Transmit to device #x
	  Wire.write(5); // Identifer to tell the slave arduinos that the next byte is the game state
	  Wire.write(state);
  	  Wire.endTransmission();
	  
	  Serial.println("DEBUG: Game state has been transmitted to a slave");
	}
}

// Function that sends the game state to slaves (true or false -> next round or game ended)
void transmitBuzzerState() {
	Wire.beginTransmission(participant); // Transmit to device #x
	Wire.write(6); // Identifer to tell the slave arduinos that the next byte is the fastest response
  	Wire.endTransmission();
	
	Serial.println("DEBUG: Buzzer state has been transmitted to a slave");
}

// Function that handles/requests the users input
void handleResponse(byte gamemodeID) {
    for (int device = 0; device < 1; device++) {
		switch(gamemodeID) {
			// Incase the gamemode is buzzer
			case 0: {
				Serial.println("DEBUG: Get slave response to buzzer question");
				
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
				Serial.println("DEBUG: Get slave response to multiple choice question");
				
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

