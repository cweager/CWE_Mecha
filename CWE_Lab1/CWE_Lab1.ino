//*************************************************************************************
//**
//**  PROGRAM:  CWE_Lab1
//**  AUTHOR:   Cody Eager, 1/c  
//**
//**  MODULE:   CWE_Lab1.ino
//**  DATE:     11 September 2014
//**
//**  DESCRIPTION:  Lab 1 is a programming exercise that explores ARDUINO setup and loop
//**                functionality by using a piezo buzzer, LCD screen, and tri colored
//**                LED.
//**
//*************************************************************************************

/*
      NOTE:  Many instances of this code was borrowed from the Lab 1 example.  These code
 pieces were written by LT Dahlen (USCGA Dept. of Electrical Engineering).  Please
 notice that the following comment shows code pieces that were fundamentally written
 by Mr. Dahlen, or borrowed similar practices:
 
 //@LT Dahlen: (explanation)
 
 */

#include <USART.h>
#include <SPI.h>
#include <line_parser.h>

/*
 * Mechatronics Lab 1.
 *
 * Copyright 2014 Aaron P. Dahlen       APDahlen@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */



/** @Warning - there is a bug somewhere between the ISR and the Arduino analogWrite
 * function.  If you need to use the PWM from within the ISR then use:
 *
 * http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM
 * 
 *    void setup(){
 *        .
 *        .
 *        .
 *        pinMode(3, OUTPUT);
 *        pinMode(11, OUTPUT);
 *        TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
 *        TCCR2B = _BV(CS22);                                               // This register controls PWM frequency
 *        OCR2A = 0;                                                        // Arduino I/O pin # 11 duty cycle
 *        OCR2B = 0;                                                        // Arduino I/O pin # 3 duty cycle
 *        .
 *        .
 *        .
 *    }
 *
 *    void loop(){
 *        .
 *        .
 *        .
 *        OCR2A = pin_11_PWM_DC;
 *        OCR2B = pin_3_PWM_DC;
 *        .
 *        .
 *        .
 *    }
 */


// AVR GCC libraries for more information see:
//      http://www.nongnu.org/avr-libc/user-manual/modules.html
//      https://www.gnu.org/software/libc/manual/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

// Arduino libraries: see http://arduino.cc/en/Reference/Libraries

#include <LiquidCrystal.h>

// Project specific includes

#include "configuration.h"
#include "USART.h"
#include "line_parser.h"

// Global variables

char line[BUF_LEN];

LiquidCrystal lcd (LCD_RS,  LCD_E, LCD_D4,  LCD_D5, LCD_D6, LCD_D7);   // Yes, this is a variable!


void setup(){

  USART_init(F_CLK, BAUD_RATE);                   // The USART code must be placed in your Arduino sketchbook
  USART_set_terminator(LINE_TERMINATOR);

  sprintf(line, "Welcome to mechatronics!\n");
  USART_puts(line);

  lcd.begin(16, 2);                               // Define LCD as a 2-line by 16 char device
  lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position
  lcd.print("Cody Eager");
  lcd.setCursor(0, 1);                            // Point to LCD line 2 left character position
  lcd.print("Houston, Texas");

  //@LT DAHLEN: Initial Piezo Startup
  tone(BUZ_PIN, 554);    // C                      // Make a happy noise
  delay(200);
  tone(BUZ_PIN, 659);    // E
  delay(100);
  tone(BUZ_PIN, 784);    // G
  delay(100);
  tone(BUZ_PIN, 1047);   // C
  delay(200);
  noTone(BUZ_PIN);

  //@LT Dahlen: Initial LED Startup
  analogWrite(TRI_LED_R, 255);
  delay(400);
  analogWrite(TRI_LED_R, 0);
  analogWrite(TRI_LED_G, 255);
  delay(400);
  analogWrite(TRI_LED_G, 0);
  analogWrite(TRI_LED_B, 255);
  delay(400);
  analogWrite(TRI_LED_B, 0);

  delay(1000);

  lcd.clear();
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
  Program:  CWE_Lab1
 Author:   Cody Eager, 1/c
 
 Module:   loop()
 Date:     08 September 2014
 
 Description:  loop() loops through the Lab 1 alpha requirements.  Some of the functionality
 includes counting from 0 to 9 using an LED screen and sounding a buzzer when the count resets.
 */

void loop(){

  //Count to 9, restart count
  for (int i = 0; i < 10; i++) {

    //set LCD Cursor to top left
    lcd.setCursor(0,0);

    //print count
    lcd.print(i);

    //audible beep for .25s at i = 0
    if (i == 0) {

      tone(BUZ_PIN, 554); 

    }

    //hold value for .5s (500 ms)
    delay(500);

    noTone(BUZ_PIN);

  }

}

