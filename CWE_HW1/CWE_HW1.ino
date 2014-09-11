//*************************************************************************************
//**
//**  PROGRAM:  CWE_HW1
//**  AUTHOR:   Cody Eager, 1/c  
//**
//**  MODULE:   CWE_HW1.ino
//**  DATE:     10 September 2014
//**
//**  DESCRIPTION:  HW1 is a programming exercise that explores ARDUINO setup and loop
//**                functionality by using a piezo buzzer, LCD screen, and tri colored
//**                LED.
//**
//*************************************************************************************

/*
    NOTE:  Many instances of this code was borrowed from the Lab 1 exercise.  These code
 pieces were written by LT Dahlen (USCGA Dept. of Electrical Engineering).  Please
 notice that the following comment shows code pieces that were fundamentally written
 by Mr. Dahlen, or borrowed similar practices:
 
 //@LT Dahlen: (explanation)
 
 */

#include <USART.h> 
#include <SPI.h>  
#include <line_parser.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <LiquidCrystal.h>

#include "configuration.h"
#include "USART.h"
#include "line_parser.h"

char line[BUF_LEN];

LiquidCrystal lcd (LCD_RS,  LCD_E, LCD_D4,  LCD_D5, LCD_D6, LCD_D7);   // Yes, this is a variable!


void setup(){

  //@LT Dahlen: the following lines are used to set up output pins, configure out LCD display, and begin HW1 task list.

  //output pins to activate pieze and LED
  pinMode(2, OUTPUT);//buzzer
  pinMode(13, OUTPUT);//led indicator when singing a note

  USART_init(F_CLK, BAUD_RATE);                   // The USART code must be placed in your Arduino sketchbook
  USART_set_terminator(LINE_TERMINATOR);

  sprintf(line, "Welcome to mechatronics!\n");
  USART_puts(line);

  //@LT Dahlen: LCD definition used in Lab1 carried over to HW1
  lcd.begin(16, 2);                               // Define LCD as a 2-line by 16 char device
  lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position

  //Print & hold information for TASK 1
  lcd.print("Cody Eager");     //My name
  lcd.setCursor(0, 1);         // Point to LCD line 2 left character position
  lcd.print("Houston, Texas"); //My hometown
  delay(2000);                 //Stay for 2 seconds

  //Scroll and clear information for TASK 2
  for (int i = 0; i < 15; i++)
  {
    lcd.scrollDisplayRight(); //scroll name to the right and off screen
    delay(34);               //name scroll should take .5s to clear
  }
  lcd.clear(); //clear LCD screen

  //Run LED through rainbow for TASK 3
  analogWrite(TRI_LED_R, 255);
  delay(500);
  analogWrite(TRI_LED_G, 128);
  delay(500);
  analogWrite(TRI_LED_G, 255);
  delay(500);
  analogWrite(TRI_LED_R, 0);
  delay(500);
  analogWrite(TRI_LED_G, 0);
  analogWrite(TRI_LED_B, 255);
  delay(500);
  analogWrite(TRI_LED_R, 128);
  delay(500);
  analogWrite(TRI_LED_R, 0);
  analogWrite(TRI_LED_B, 0);

  //Play super mario theme song for TASK 4
  play_mario(); //play super mario theme song
   
   //Impersonate R2D2 for TASK 5
   int totDur = 0; //impersinate R2D2 for five seconds (5,000 ms)
   while (totDur < 5000) {
   
   long ranTone = random(100, 4500);
   long ranDur = random(50, 200);
   tone(BUZ_PIN, ranTone); //play random R2D2 noise
   delay(ranDur);
   totDur += ranDur;
   } 
   noTone(BUZ_PIN);

}

//Super Mario Bro's Theme Song Function
//Program:  HW1
//Author:   PRINCE
//Citation: http://www.princetronics.com/supermariothemesong/
//
//Editor:   Cody Eager, 1/c
//Module:   play_mario()
//Date:     09 September 2014
void play_mario() {

  int melody[] = {
    2637, 2637, 0, 2637,
    0, 2093, 2637, 0,
    3136, 0, 0,  0,
    1568, 0, 0, 0,

    2093, 0, 0, 1568,
    0, 0, 1319, 0,
    0, 1760, 0, 1976,
    0, 1865, 1760, 0,

    1568, 2637, 3136,
    3520, 0, 2794, 3136,
    0, 2637, 0, 2093,
    2349, 1976, 0, 0,

    2093, 0, 0, 1568,
    0, 0, 1319, 0,
    0, 1760, 0, 1976,
    0, 1865, 1760, 0,

    1568, 2637, 3136,
    3520, 0, 2794, 3136,
    0, 2637, 0, 2093,
    2349, 1976, 0, 0
  };

  int tempo[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
  };

  for (int thisNote = 0; thisNote < 78; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempo[thisNote];

    buzz(BUZ_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(BUZ_PIN, 0, noteDuration);
  }  
}

void buzz(int targetPin, long frequency, long length) {
  digitalWrite(13, HIGH);
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }  
  digitalWrite(13, LOW);  
}

/*********************************************************************************
 *  ______  ____   _____   ______  _____  _____    ____   _    _  _   _  _____
 * |  ____|/ __ \ |  __ \ |  ____|/ ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |__  | |  | || |__) || |__  | |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  __| | |  | ||  _  / |  __| | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |    | |__| || | \ \ | |____| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |_|     \____/ |_|  \_\|______|\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 ********************************************************************************/

ISR(USART_RX_vect){

  /**
   * @note This Interrupt Service Routine is called when a new character is received by the USART.
   * Idealy it would have been placed in the USART.cpp file but there is a error "multiple definition 
   * of vector_18".  Apparently Arduino detects when an ISR is in the main sketch.  If you place it 
   * somewhere else it is missed and replaced with the Arduino handler.  This is the source of the 
   * multiple definitions error -  * see discussion @ http://forum.arduino.cc/index.php?topic=42153.0
   */
  USART_handle_ISR();
}




/*********************************************************************************
 *  ____            _____  _  __ _____  _____    ____   _    _  _   _  _____
 * |  _ \    /\    / ____|| |/ // ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |_) |  /  \  | |     | ' /| |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  _ <  / /\ \ | |     |  < | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |_) |/ ____ \| |____ | . \| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |____//_/    \_\\_____||_|\_\\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 ********************************************************************************/

/*
  Program:  CWE_HW1
 Author:   Cody Eager, 1/c
 
 Module:   loop()
 Date:     09 September 2014
 
 Description:  loop() displays normalized values received by user inputs to the joystick and changes the color of an
 LED as the joystick is moved.
 */

void loop(){

  //@LT DAHLEN: the following definitinos and use of statics were borrowed from Lab 1 exercise

  int joy_vert, joy_horz;

  static uint32_t next_LCD_update_time = 0; 

  uint32_t current_time;

  // Display the joystick values on the LCD
  current_time = millis();                        
  if(current_time >= next_LCD_update_time){
    next_LCD_update_time = current_time + 200;

    //Normalize values IAW TASK 6
    //NOTE: JOY_HORZ pin switched with JOY_VERT pin for easier user application
    joy_vert = analogRead(JOY_HORZ)/5.12;
    joy_horz = analogRead(JOY_VERT)/5.12;

    joy_vert = round(joy_vert)-100;                  
    joy_horz = -round(joy_horz)+99;

    snprintf(line, 17, "V =%4d, H =%4d", joy_vert, joy_horz);
    lcd.setCursor(0, 0);
    lcd.print(line);

    //Normalize LED colors to joy stick inputs for TASK 7 
    if (joy_vert > 0) {
      analogWrite(TRI_LED_B, 0);
      analogWrite(TRI_LED_G, 0);
      analogWrite(TRI_LED_R, joy_vert*2.55);
    }
    else if (joy_vert < 0 ) {
      analogWrite(TRI_LED_B, -joy_vert*2.55);
      analogWrite(TRI_LED_R, 0);
      analogWrite(TRI_LED_G, 0);
    }
    else if (joy_horz > 0) {
      analogWrite(TRI_LED_G, joy_horz*2.55);
      analogWrite(TRI_LED_B, 0);
      analogWrite(TRI_LED_R, 0);
    }
    else if (joy_horz < 0) {
      analogWrite(TRI_LED_G, (joy_horz*2.55)+255);
      analogWrite(TRI_LED_R, (joy_horz*2.55)+255);
      analogWrite(TRI_LED_B, (joy_horz*2.55)+255);
    }
    else {
      analogWrite(TRI_LED_G, 255);
      analogWrite(TRI_LED_R, 255);
      analogWrite(TRI_LED_B, 255); 
    }
  }
}



