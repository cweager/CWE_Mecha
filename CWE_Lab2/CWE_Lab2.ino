//*****************************************************************************
//**
//**  PROGRAM:  CWE_Lab2
//**  AUTHOR:   Cody Eager, 1/c
//**
//**  MODULE:   CWE_Lab2.ino
//**  DATE:     18 September 2014
//**
//**  DESCRIPTION:  Lab2 is a follow on lab from Lab1 and demonstrate step
//**                start control over DC motors.
//**
//*****************************************************************************

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

#include "USART.h"
#include "line_parser.h"
#include "configuration.h"

// Global variables

char line[BUF_LEN];

LiquidCrystal lcd (LCD_RS,  LCD_E, LCD_D4,  LCD_D5, LCD_D6, LCD_D7);   // Yes, this is a variable!

void setup(){

  //@LT Dahlen: the following lines are used to set up output pins, configure out LCD display, and begin Lab2 task list.

  //output pins to activate piezo and LED
  pinMode(2, OUTPUT);//buzzer
  pinMode(13, OUTPUT);//led indicator when singing a note
  pinMode(11, OUTPUT);//K2 Relay
  pinMode(3, OUTPUT);//K1 Relay

  USART_init(F_CLK, BAUD_RATE);                   // The USART code must be placed in your Arduino sketchbook
  USART_set_terminator(LINE_TERMINATOR);

  sprintf(line, "This lab demonstrates control over DC motors.\n");
  USART_puts(line);                              //Send line to USART

  //@LT Dahlen: LCD definition used in Lab1 carried over
  lcd.begin(16, 2);                               // Define LCD as a 2-line by 16 char device
  lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position

  lcd.print("Step Start Lab");
  lcd.setCursor(0, 1);         // Point to LCD line 2 left character position
  lcd.print("Cody Eager"); 

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

void loop(){
  
  unsigned long elapsedTime;
  
  elapsedTime = millis();     //get time since program started
 
 if (elapsedTime == 3000){    //after 3seconds, activate motor sequence
   
    sprintf(line, "Standby, a motor start sequence has been initiated.\n");
    USART_puts(line);                               //Send line to USART
    
    lcd.clear();                                    // Clear LCD
    lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position
    lcd.print("CAUTION!");
    lcd.setCursor(0, 1);                            // Point to LCD line 2 left character position
    lcd.print("Motor Starting.");
    
    tone(BUZ_PIN, 5000);
    delay(250);
    noTone(BUZ_PIN);
    delay(250);
    tone(BUZ_PIN, 5000);
    delay(250);
    noTone(BUZ_PIN);
    delay(250);
    tone(BUZ_PIN, 5000);
    delay(250);
    noTone(BUZ_PIN);
   
 }
 
 else if (elapsedTime == 6000){
  
    sprintf(line, "Motor is accelerating.\n");
    USART_puts(line);                               //Send line to USART
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor ramping.");
    
    // Activate K2 relay (pin 11)
    digitalWrite(K2, HIGH);
    
 }
 
 else if (elapsedTime == 9000){
  
    sprintf(line, "Full power engaged.\n");
    USART_puts(line);                               //Send line to USART
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor running.");
    
    // Activate K1 relay (pin 3)
    digitalWrite(K1, HIGH);
    // Deactivate K2 relay (pin 11)
    digitalWrite(K2, LOW);
    
 }
 
 else if (elapsedTime == 12000){
  
    sprintf(line, "Motor secured and coasting to a stop.\n");
    USART_puts(line);                               //Send line to USART
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor secured.");
    
    // Deactivate K1 relay (pin 3)
    digitalWrite(K1, LOW);
    // Deactivate K2 relay (pin 11)
    digitalWrite(K2, LOW);
    
    tone(BUZ_PIN, 7000);
    delay(500);
    noTone(BUZ_PIN);
    
 }
  
}

