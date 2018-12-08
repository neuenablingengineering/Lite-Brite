/*
 Cycles through different display options on a 16 x n LCD display by clicking on button, and selects option using the other button. The options are:

Textual Recognition: Meant to develop textual recognition skills by displaying the color of the peg that needs to be inserted and then sensing if the correct color has been inserted, and provide the appropriate feedback.

Audio Recognition: Says the color that needs to be inserted (e.g. “Insert Blue Peg”) and then senses if the correct color has been inserted, and provides the appropriate feedback.

Motor Skills: Meant to develop simple motor skills (i.e. being able to insert the peg in the hole), senses if the peg has been inserted and provides positive feedback once done.
 
  LCD Connections:
 * LCD RS - 12
 * LCD Enable - 11
 * LCD D4 - 5
 * LCD D5 - 4
 * LCD D6 - 3
 * LCD D7 - 2
 * LCD R/W - grnd
 * LCD VSS - grnd
 * LCD VCC - 5V
 * LED Backlight - 5V+220(ohm) ; grnd
 
 Button Connections:
 *Pin 8 - Input
 *Grnd
 
 Selection Button:
 *Pin 9 - Input
 *Grnd


**Color sensor capabilities

 

By: Luigi Annese - professional basket weaver
 */

// LCD Library
#include <LiquidCrystal.h>
#include <Wire.h>                   //color sensor
#include "Adafruit_TCS34725.h"      //color sensor

// initialize LCD library with appropriate pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//initialize color sensor with default values (int time = 2.4ms, gain = 1x)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

//constants
const int reset = 7;  //to reset
const int bpin = 9; //button pin input
const int sbpin = 8; //selection button pin input
const int light = 13; //light pin in board

//variables
int pushcount = 0;       //keeps track of pushes 
int currentbstate = 0;   //current button position [pushed = HIGH; unpushed = LOW]
int lastbstate = 0;      //to detect change in position
int sstate = 0;          //keeps track of option selection - selection button
int scurrentbstate = 0;  //current button position - selection button
int slastbstate = 0 ;    //detect change in position - selection button
int out;                 //for exiting while loops without completion
char* selection;
char* peg;


void setup() {
  pinMode(bpin, INPUT);
  pinMode(sbpin, INPUT); //selection button input
  pinMode(light, OUTPUT);
  pinMode(reset, OUTPUT);
  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  lcd.print("Main Menu"); //initial message

  //color sensor
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... wtf?");
    while(1);
  }
}

void loop() {
  currentbstate = digitalRead(bpin); //reads current state
  scurrentbstate = digitalRead(sbpin); //reads current state - selection button

  //compares current with previous state - adds a count if pushed & displays menu setting
  if (currentbstate != lastbstate && currentbstate == HIGH){
    pushcount++;
    delay(50);     //to avoid bouncing
   
      //lcd.setCursor(9,0);
    lcd.clear();
    if(pushcount % 3 == 1){
      lcd.print("Textual");
    } else if(pushcount % 3 == 2){
      lcd.print("Audio");
    } else if(pushcount % 3 == 0) {
      lcd.print("Motor");
    }
  }
  
  lastbstate = currentbstate;  //resets last state
  
  
  //selection button
  if(scurrentbstate != slastbstate && scurrentbstate == HIGH && pushcount != 0){
    if(pushcount % 3 == 1){
      //lcd.setCursor(0,1); 
      lcd.clear();
      selection = randomcolor(selection);  
      lcd.print("Insert "); 
      lcd.print(selection); 
      lcd.print(" Peg");
      out = 0;

      while (scurrentbstate == HIGH) {
        scurrentbstate = digitalRead(sbpin);
        //delay(50);
      }
      //checks if inserted peg is correct color - allows an exit option
      while(peg != selection && out < 20) {
        peg = colorsensor(peg);
        //peg = 'x';
        out = 0;
        scurrentbstate = digitalRead(sbpin);
        while (scurrentbstate == HIGH && out < 20) {        //exit mode by leaving button pressed for 2 seconds
          out++;
          delay(100);
          scurrentbstate = digitalRead(sbpin);
          }
      //delay (50);
      }
    
    pfeedback(out);  //positive feedback if successfull
    out = 0;
    pushcount = 0;
    lastbstate = 0;
    slastbstate = 0;
    peg[0] = 0;
    delay(2000);
    lcd.clear();
    lcd.print("Main Menu");


    } else if(pushcount % 3 == 2){     //Audio Recognition - Will say a color and detect the color of the peg to give feedback
        lcd.clear();
        selection = randomcolor(selection);  
        lcd.print("Insert "); 
        lcd.print(selection); 
        lcd.print(" Peg");
        out = 0;

        //audio(selection);              //Tells (i.e. talks)wq user to insert appropiate peg

        while (scurrentbstate == HIGH) {
          scurrentbstate = digitalRead(sbpin);
          delay(50);
        }
        //checks if inserted peg is correct color - allows an exit option
        while(peg != selection && out < 20) {
          peg = colorsensor(peg);
          //peg = 'x';
          out = 0;
          scurrentbstate = digitalRead(sbpin);
          while (scurrentbstate == HIGH && out < 20) {        //exit mode by leaving button pressed for 2 seconds
            out++;
            delay(100);
            scurrentbstate = digitalRead(sbpin);
            }
        delay (50);
        }
    
      pfeedback(out);                   //positive feedback if successfull
      out = 0;                          //reset variables
      pushcount = 0;
      lastbstate = 0;
      slastbstate = 0;
      peg[0] = 0;
      delay(2000);
      lcd.clear();
      lcd.print("Main Menu");
    
    } else if(pushcount % 3 == 0){
        lcd.clear();
        lcd.print("Motor Selected");      //For developing motor skill, will detect if a peg is inserted and give positive feedback
        delay(2000);
        lcd.clear();
        lcd.print("Main Menu");
    } delay(50);
  }
}

char* randomcolor(char* selection){
    int color = random(1,4);
      if(color == 1){
        selection = "Red";
      } else if(color == 2) {
        selection = "Blue";
      } else if(color == 3){
        selection = "Yellow";
      }
  return selection;      
}      

void pfeedback(int check){      //Textual Recognition - LCD shows color (e.g. ‘Red’) and detects the color of the peg to give feedback.
  lcd.clear();
  if (check < 20){              //successful
    lcd.print("you are free");
  } else {                      //manual exit
    lcd.print("TASK fled!");
  } 
}

char* colorsensor(char* color) {
  uint16_t r, g, b, c, colorTemp, lux;

  //get color values
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);

  //Blue
  if (b > 2500 && g < 2500 && r < 2500) {
    color = "Blue";
  } else if (g > 5000 && r > 5000) {
    color = "Yellow";
  } else if (b < 2500 && g < 2500 && r > 3500) { 
    color = "Red";
  } 
  return color;
}

