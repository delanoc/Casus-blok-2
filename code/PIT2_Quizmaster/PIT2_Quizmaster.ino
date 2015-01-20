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
#include <string.h>

// ---------- Constants
#define VERSION 1
#define MAX_ARRAY_SIZE 40

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
	char question[80];
	char answer[24];
	int correctScore;
	int incorrectScore;
};

struct buzzerQuestions buzQ1;

// Game and round variables
int maxQuestions; // Maximum amount of questions
int gamemodeID; // Integer to determine gamemode
int correctAnswer; // Buzzer question score (add or take points)

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
	
	// buzzerQuestion 1 specification
	strcpy(buzQ1.question, "Wat is de hoofdstad van Nederland?");
	strcpy(buzQ1.answer, "Amsterdam");
	buzQ1.correctScore = 4;
	buzQ1.incorrectScore = -2;
	
	// Game round starts
	int gameRound = 0; // Init game round at 0
	int questionNum = 1;
	while (gameRound < maxQuestions) {
		gamemodeMenu(); // Open the menu to choose the gamemode for one specific round
		
		// If gamemode is buzzer
		if (gamemodeID == 0) {
			
			int questionIndex = random(0, 20); // Get a random number to use as an index in the question and score array
			
			// Display question
			lcd.clear();
			
			// First row
			displayQuestionNum(questionNum); // Display question in first row
			delay(1000);
			
			// Second row
			lcd.clear();
			lcd.setCursor(5, 0);
			lcd.print(buzQ1.question);			
			
			lcdScroll(); // Scroll through the question
			
			
			/* ---------------------------------------------------
				SEND QUESTIONINDEX TO SLAVES
				REQUEST USER INPUT
				GET USE INPUT
			 --------------------------------------------------- */
			
			
			// Display correct answer
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Antwoord model:");
			lcd.setCursor(0, 1);
			lcd.print(buzQ1.answer);
			
			delay(2500);
			
			// Give points or take points from participant
			lcd.clear();
			buzzerMenu();
			
			
			questionNum++; // Next question number
		}
		// If gamemode is multiple choice
		else if (gamemodeID == 1) {
			
			int questionIndex = random(20, 40); // Stuur naar andere slaves
			
		}
		
		gameRound++; // Proceed to next round
	}
}

// Function to display the question number relative to the max questions
void displayQuestionNum(int currentQuestion) {
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
		delay(350);
	}
}

// Function that prompts the quizmaster to choose the amount of questions
void questionsMenu() {
    boolean selected = false;
    
	maxQuestions = 5; // The minimum amount of questions
	
    while (!selected) {
		
        boolean pressed = false;
        while (!pressed) {
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
            delay(250);
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
			
			// Print the value to the screen
            lcd.clear();
            lcd.print("Goed antwoord?");
            lcd.setCursor(0, 1);
			
			if (correctAnswer == 0) lcd.print("Incorrect");
			else if (correctAnswer == 1) lcd.print("Correct");
			
			// Prevent overflow buttons
            delay(250);
        }
    }
}


/*
// Function that generates random numbers and stores them into the array
void generateQuestion() {
	for (int i = 0; i < amount; i++) {
		questionArray[i] = random(20); // Fill the entire array based on max storage
	}
}*/

/*
// Function that determines the amount of time in between sorts
void duration(int start, int end, char* algorithm) {
	int timeDifference = end - start; // Get the duration of the algorithm
	
	lcd.clear();
	
	// LCD print to show the duration
	lcd.setCursor(0, 0); // Set the cursor to the first column and the first row
	lcd.print(algorithm); // Set the algorithm name
	lcd.setCursor(11, 0); // Set the cursor to the eleventh column and the first row
	lcd.print("time:"); // Set extra title name
	lcd.setCursor(0, 1); // Set the cursor to the first column and the second row
	lcd.print(timeDifference); // Show the amount
	lcd.setCursor(10, 1); // Set the cursor to the tenth column and the second row
	lcd.print("millis"); // Milliseconds
	
	// The purpose of this delay is to make sure the duration of the sort is displayed on the LCD screen.
	// This is not the most convenient / efficient way to do this.
	delay(1000);
}*/
