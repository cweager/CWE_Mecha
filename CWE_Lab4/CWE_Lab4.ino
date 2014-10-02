//*****************************************************************************
//**
//**  PROGRAM:  CWE_Lab4
//**  AUTHOR:   Cody Eager, 1/c
//**
//**  MODULE:   CWE_Lab4.ino
//**  DATE:     02 October 2014
//**
//**  DESCRIPTION:  Lab4 is a follow on lab from previous labs.  The purpose of
//**                this lab is to replicate the same functionality as labs in
//**                the past, but do so utilizing finite state machines.
//**
//*****************************************************************************

/*
    NOTE:  Many instances of this code was borrowed from the Lab 1 exercise.  These code
 pieces were written by LT Dahlen (USCGA Dept. of Electrical Engineering).  Please
 notice that the following comment shows code pieces that were fundamentally written
 by Mr. Dahlen, or borrowed similar practices:
 
 //@LT Dahlen: (explanation)
 
 */
/*
 * Finite State Machine (FSM) and Interrupt Service Routine (ISR) template.
 *
 * Copyright 2014 Aaron P. Dahlen 
 *
 *     APDahlen@gmail.com
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

/** @Warning - there is a bug between the timer ISR and the Arduino analogWrite
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
//     http://www.nongnu.org/avr-libc/user-manual/modules.html
//     https://www.gnu.org/software/libc/manual/

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



// Global variables

char line[BUF_LEN];
char LCD_str[17];

LiquidCrystal lcd (LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);   // Yes, this is a variable!

volatile uint8_t reset_flag = 0;
volatile uint8_t start_flag = 0;
volatile uint8_t idle_flag = 0;
volatile uint8_t engage_flag = 0;
volatile uint8_t lowpower_flag = 0;
volatile uint8_t fault_flag = 0;

typedef enum{ 
  initial, secured, startup, idle, full, lowpower, fault } 
e_states;

volatile static e_states state = initial;



void setup(){

  digitalWrite(K1_PIN, LOW);
  digitalWrite(K2_PIN, LOW);
  pinMode(K1_PIN, OUTPUT);
  pinMode(K2_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT); 

  USART_init(F_CLK, BAUD_RATE);                   // The USART code must be placed in your Arduino sketchbook
  USART_set_terminator(LINE_TERMINATOR);

  init_timer_1_CTC(100);                          // Enable the timer ISR

  lcd.begin(16, 2);   // Initialize LCD display
  
  /*pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22);                                               // This register controls PWM frequency
  OCR2A = 0; //SET PWM OF K2 (11)
  OCR2B = 0; //SET PWM OF K1 (3)*/

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




ISR(TIMER1_COMPA_vect){

  /** @brief This Interrupt Service Routine (ISR) serves as the 100 Hz heartbeat 
   * for the Arduino.  See the companion function init_timer_1_CTC for additional 
   * information.
   *
   * @ Note:
   *    1) Compiler generated code pushes status register and any used registers to stack.
   *    2) Calling a subroutine from the ISR causes compiler to save all 32 registers; a 
   *       slow operation (fact check). 
   *    3) Status and used registers are popped by compiler generated code.
   */

  static uint8_t LED_flag;
  static uint16_t time_in_state = 0;


  //  INSERT one second LED blink code here...
  static uint8_t LED_count = 0;
  LED_count++;

  if (LED_count <= 49){

    digitalWrite(LED_PIN, HIGH);

  }
  else{

    digitalWrite(LED_PIN, LOW);
    if (LED_count == 99){

      LED_count = 0;

    }
  }


  switch(state){

  case initial:

    if(++time_in_state > 250){
      state = secured;
      time_in_state = 0;
    }

    //De-energize realys
    digitalWrite(K1_PIN, LOW);
    digitalWrite(K2_PIN, LOW);
    //OCR2A = 128; //SET PWM OF K2 (11)
    //OCR2B = 128; //SET PWM OF K1 (3)

    break;

  case secured:

    digitalWrite(K1_PIN, LOW);
    digitalWrite(K2_PIN, LOW);
    //OCR2A = 128; //SET PWM OF K2 (11)
    //OCR2B = 128; //SET PWM OF K1 (3)
    if (start_flag){
      start_flag = 0;
      state = startup;
      time_in_state = 0;
    }

    break;
    
  case startup:
    digitalWrite(K1_PIN, LOW);
    digitalWrite(K2_PIN, HIGH);
    //OCR2A = 255; //SET PWM OF K2 (11)
    //OCR2B = 0; //SET PWM OF K1 (3)
    if (++time_in_state > 400){
      //fault_flag = 0;
      state = fault;
      time_in_state = 0;
    }
    else if (idle_flag){
      idle_flag = 0;
      state = idle;
      time_in_state = 0;
    }
    break;

  case idle:

    digitalWrite(K1_PIN, LOW);
    digitalWrite(K2_PIN, HIGH);
    //OCR2A = 255; //SET PWM OF K2 (11)
    //OCR2B = 0; //SET PWM OF K1 (3)
    if (engage_flag){
      engage_flag = 0;
      state = full;
      time_in_state = 0;
    }
    break;

  case full:

    digitalWrite(K1_PIN, HIGH);
    digitalWrite(K2_PIN, LOW);
    //OCR2A = 0; //SET PWM OF K2 (11)
    //OCR2B = 255; //SET PWM OF K1 (3)
    if (++time_in_state > 100) {
      state = lowpower;
      time_in_state = 0;
    }

    break;
    
  case lowpower:
  
    //Reduce K1 PWM to lowest possible
    //OCR2A = 0; //SET PWM OF K2 (11)
    //OCR2B = 128; //SET PWM OF K1 (3)
    if (++time_in_state > 200) {
      state = secured;
      time_in_state = 0;
    }
    
    break;
    
  case fault:    
    digitalWrite(K1_PIN, LOW);
    digitalWrite(K2_PIN, LOW);

    if (reset_flag){
      noTone(BUZ_PIN);
      reset_flag = 0;
      state = secured;
      time_in_state = 0;
    }    
    break;


  default:
    state = secured;
    break; 
  }
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

  char buf[50];
  static uint8_t last_state = !initial;
  static uint32_t RPM = 0; //RPM (DC Voltage) of our Tachometer
  static uint32_t count = 0; //Counter to track number of times go through loop

  /* This section of code executes once on change of state.  It is useful for
   * the LCD display and to send messages via the USART.
   */

  if(state != last_state){
    lcd.clear();

    switch(state){

    case initial:
      lcd.clear();
      lcd.print("Mecha FSM Lab");
      lcd.setCursor(0, 1);
      lcd.print("  02 Oct 14");

      USART_puts("This lab demonstrates serial control of the Arduino. It also changes motor operating\n");
      USART_puts("states based on feedback from a tachometer.  Type \"start,\" then strike enter to begin.\n");

      tone(BUZ_PIN, 554);    // C                      // Make a happy noise
      delay(200);
      tone(BUZ_PIN, 659);    // E
      delay(100);
      tone(BUZ_PIN, 784);    // G
      delay(100);
      tone(BUZ_PIN, 1047);   // C
      delay(200);
      noTone(BUZ_PIN);

      delay(500);
      break;


    case secured:
    if (last_state == lowpower) {
      USART_puts("\nMotor secured.\n");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Motor secured.");
      delay(3000);
    }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("State:  Secured");
      sprintf(line, "Please type start to initiate a motor start.                  ");

      for(int i = 0; i < 50; i++){

        snprintf(LCD_str, 17, line+i);

        lcd.setCursor(0, 1);
        lcd.print(LCD_str);
        delay(150);

      }

      USART_puts("\nType \"START\" to initiate a startup sequence.\n");
      break;
      
    case startup:
    
      USART_puts("Standby! a motor start sequence has been initiated.\n");

      lcd.clear();                                    // Clear LCD
      lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position
      lcd.print("CAUTION!");
      lcd.setCursor(0, 1);                            // Point to LCD line 2 left character position
      lcd.print("Motor Starting.");
      USART_puts("Motor is ramping.\n");

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

      //Scroll and clear message
      for (int i = 0; i < 15; i++)
      {
        lcd.scrollDisplayRight(); 
        delay(50);
      }
      lcd.clear(); //clear LCD screen
      
      lcd.setCursor(0,0);
      lcd.print("State: Startup"); 
      break;


    case idle:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("State: Idle");

      tone(BUZ_PIN, 1047);   // C
      delay(200);
      noTone(BUZ_PIN);
      
      USART_puts("\nMotor is in idle state.\n");
      USART_puts("Type \"ENGAGE\" to go to full power.\n");
      break;


    case full:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("State:  Full");

      USART_puts("Motor is in full power state.\n");

      tone(BUZ_PIN, 1047);   // C
      delay(200);
      noTone(BUZ_PIN);
      break;
      
    case lowpower:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("State:  PWM");

      USART_puts("\nRelay K1 duty cycle reduced to save power.\n");
      USART_puts("Motor will secure in 2 seconds.\n");

      tone(BUZ_PIN, 1047);   // C
      delay(200);
      noTone(BUZ_PIN);   
      break;
      
    case fault:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("State:  Fault");
      
      USART_puts("\nCAUTION! Motor did not achieve idle!\n");
      USART_puts("Type \"RESET\" to reset system.\n");
      break;


    default:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("State:  Unkown");
      break;
    }
    last_state = state;
  }

  /* This section of code is used to receive messages from the USART.  If a messages
   * is received a flag is set signaling the ISR to perform the operation.  This construct
   * was used to keep the ISR code small and fast.
   */

  switch(state){  

  case initial:
    break;

  case secured:
    if(USART_is_string()){
      USART_gets(buf);
      if(!strcmp(buf,"start") || !strcmp(buf,"Start") || !strcmp(buf,"START")){
        start_flag = 1;
        USART_puts(strcat(buf, "\n\n"));
      }
      else {
        USART_puts("\nInvalid command.  Please type \"START.\"\n"); 
      }
    }
    break;
    
  case startup:
    RPM = analogRead(TACH_PIN);
    sprintf(LCD_str, "RPM: %u ", RPM);
    
    lcd.setCursor(0, 1);
    lcd.print(LCD_str);
    
    if (count > 10) {
      USART_puts(strcat(LCD_str, "\n"));
      count = 0;
    }
    
    if(RPM > 275) {
      idle_flag = 1;
    }
    
    count++;
    delay(50);
    break;

  case idle:
    RPM = analogRead(TACH_PIN);
    sprintf(LCD_str, "RPM: %u ", RPM);
    
    lcd.setCursor(0, 1);
    lcd.print(LCD_str);

    if (count > 10) {
      USART_puts(strcat(LCD_str, "\n"));
      count = 0;
    }

    if(USART_is_string()){
      USART_gets(buf);
      if(!strcmp(buf, "Engage") || !strcmp(buf, "engage") || !strcmp(buf, "ENGAGE")){
        engage_flag = 1;
        USART_puts(strcat(buf, "\n\n"));
      }
      else {
        USART_puts("\nInvalid command.  Please type \"ENGAGE.\"\n");
      }
    }

    count++;
    delay(50);
    break;


  case full:
    RPM = analogRead(TACH_PIN);
    sprintf(LCD_str, "RPM: %u ", RPM);
    lcd.setCursor(0, 1);
    lcd.print(LCD_str);

    if (count > 5) {
      USART_puts(strcat(LCD_str, "\n"));
      count = 0;
    }

    count++;
    delay(25);
    break;
    
  case lowpower:
    RPM = analogRead(TACH_PIN);
    sprintf(LCD_str, "RPM: %u ", RPM);
    lcd.setCursor(0, 1);
    lcd.print(LCD_str);

    if (count > 5) {
      USART_puts(strcat(LCD_str, "\n"));
      count = 0;
    }

    count++;
    delay(25);
    break;
  
  case fault:
    if (count < 5000) {
      tone(BUZ_PIN, 554);
    }
    else if (count > 10000) {
      count = 0;
    }
    else {
      noTone(BUZ_PIN); 
    }
    if(USART_is_string()){
      USART_gets(buf);
      if(!strcmp(buf,"reset") || !strcmp(buf,"Reset") || !strcmp(buf,"RESET")){
        reset_flag = 1;
        USART_puts(strcat(buf, "\n\n"));
      }
      else {
        USART_puts("\nInvalid command.  Please type \"RESET.\"\n"); 
      }
    }
    count++;
    noTone(BUZ_PIN);
    break;

  default:
    break;

  }
}



void init_timer_1_CTC(long desired_ISR_freq){
  /**
   * @brief Configure timer #1 to operate in Clear Timer on Capture Match (CTC Mode)
   *
   *      desired_ISR_freq = (F_CLK / prescale value) /  Output Compare Registers
   *
   *   For example:
   *        Given an Arduino Uno: F_clk = 16 MHz
   *        let prescale                = 64
   *        let desired ISR heartbeat   = 100 Hz
   *
   *        if follows that OCR1A = 2500
   *
   * @param desired_ISR_freq is the desired operating frequency of the ISR
   * @param F_CLK must be defined globally e.g., #define F_CLK 16000000L
   *
   * @return void
   *
   * @note The prescale value is set manually in this function.  Refer to ATMEL ATmega48A/PA/88A/PA/168A/PA/328/P datasheet for specific settings.
   *
   * @warning There are no checks on the desired_ISR_freq parameter.  Use this function with caution.
   *
   * @warning Use of this code will break the Arduino Servo() library.
   */
  cli();                                          // Disable global
  TCCR1A = 0;                                     // Clear timer counter control registers.  The initial value is zero but it appears Arduino code modifies them on startup...
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);                         // Timer #1 using CTC mode
  TIMSK1 |= (1 << OCIE1A);                        // Enable CTC interrupt
  TCCR1B |= (1 << CS10)|(1 << CS11);              // Prescale: divide by F_CLK by 64.  Note SC12 already cleared
  OCR1A = (F_CLK / 64L) / desired_ISR_freq;       // Interrupt when TCNT1 equals the top value of the counter specified by OCR
  sei();                                          // Enable global
}


















