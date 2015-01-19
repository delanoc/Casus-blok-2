/*
casus opdracht PIT2: quizmaster systeem
datum: januari 2015
auteurs:  - Dennis Kooij
          - Delano Cörvers
          - Davy Heutemakers
          - Camiel Kerkhofs
*/

//------- code voor "slave" arduino ------


#include <LiquidCrystal.h> //library voor gebruik van 16-pin lcd scherm

//interface pins:
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //verbonden met lcd
int button1 = 5;
int button2 = 4;
int button3 = 3;
int button4 = 2;


void setup() {
  lcd.begin(16, 2);//configureer lcd als 2 rijen met ieder 16 vakken
  
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
}


void loop() {
  
  //format:
  lcd.clear(); lcd.print(F("")); lcd.setCursor(0, 1); lcd.print("w");
  
}
