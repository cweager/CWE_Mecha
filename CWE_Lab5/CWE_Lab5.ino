//*************************************************************************************
//**
//**  PROGRAM:  CWE_Lab5
//**  AUTHOR:   Cody Eager, 1/c  
//**
//**  MODULE:   CWE_Lab5.ino
//**  DATE:     16 October 2014
//**
//**  DESCRIPTION:  Lab 5 is a group exercise that explores transistor operation, data
//**                capturing techniques, data plotting using MATLAB, and filtering.
//**
//*************************************************************************************




/*

 *This program is free software: you can redistribute it and/or modify it under the terms
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


#include <USART.h>
#include <SPI.h>
#include <line_parser.h>


/*

 * @Warning - there is a bug somewhere between the ISR and the Arduino analogWrite
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

// Project specific headers

#include "configuration.h"
#include "USART.h"
#include "line_parser.h"

// Global variables

char line[BUF_LEN];

LiquidCrystal lcd (LCD_RS,  LCD_E, LCD_D4,  LCD_D5, LCD_D6, LCD_D7);   //LCD Display

void setup(){

  USART_init(F_CLK, BAUD_RATE);
  USART_set_terminator(LINE_TERMINATOR);
  
  // Configure output pins
  
  pinMode(9, OUTPUT); // Collector supply
  pinMode(10, OUTPUT); // Base supply

  USART_puts("//*************************************************************************************\n");
  USART_puts("//**\n");
  USART_puts("//**  PROGRAM:  CWE_Lab5\n//**  AUTHOR:   Cody Eager, 1/c\n");
  USART_puts("//**\n");
  USART_puts("//**  MODULE:   CWE_Lab5.ino\n//**  DATE:     16 October 2014\n");
  USART_puts("//**\n");
  USART_puts("//**  DESCRIPTION:  Lab 5 is a group exercise that explores transistor operation, data\n");
  USART_puts("//**                capturing techniques, data plotting using MATLAB, and filtering.\n");
  USART_puts("//**\n");
  USART_puts("//*************************************************************************************\n");

  lcd.begin(16, 2);                               // Define LCD as a 2-line by 16 char device
  lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position
  lcd.print("Cody Eager");
  lcd.setCursor(0, 1);                            // Point to LCD line 2 left character position
  lcd.print("  Lab  5 ");


  tone(BUZ_PIN, 554);    // C
  delay(200);
  tone(BUZ_PIN, 659);    // E
  delay(100);
  tone(BUZ_PIN, 784);    // G
  delay(100);
  tone(BUZ_PIN, 1047);   // C
  delay(200);
  noTone(BUZ_PIN);
  
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

  static float I_c = 0;  // Collector current calculated
  static float I_b = 0;  // Base current calculated
  
  static int D_b = 0;  // Base duty cycle
  static int D_c = 0;  // Collector duty cycle
  
  static float V_bs = 0;  // Base voltage supplied (10)
  static float V_cs = 0;  // Collector voltage supplie (9)
  
  static float V_br = 0;  // Base voltage read-in (A0)
  static float V_cr = 0;  // Current voltage reade in (A1)
  
  static uint8_t ctr = 0;  // Counter
  
  // Calculate values
  
  
 
  // Get user commands from USART
  
  if ( USART_is_string() ){   
    USART_puts("\nCommand Detected.\n");
    USART_gets(line);
    USART_puts(strcat(line, "\n\n"));
    
    //retrieve I_c and I_b
    if ( strcmp(line, "?IB") ){
      USART_puts(" **I_b = ");
      USART_puts(dtostre(I_b));
      USART_puts("\n");
    }
    else if ( strcmp(line, "?IC") ){
      USART_puts(" **I_c = ");
      //USART_puts(dtostre(I_c);
      USART_puts("\n");
    }
    else{
      USART_puts("\nInvalid command.\n");
    }
    
  }
  
}

