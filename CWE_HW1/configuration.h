//*************************************************************************************
  //**
  //**  PROGRAM:  CWE_HW1
  //**  AUTHOR:   LT Dahlen 
  //**
  //**  MODULE:   configuration.h
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
  
//@LT Dahlen:  This header file was borrowed for use in HW1 from the Lab 1 exercise.

#ifndef configuration_H
    #define configuration_H

    #define F_CLK 16000000UL
    #define BAUD_RATE 9600L

    #define LINE_TERMINATOR 0x0A    // ASCII Line Feed

    #define BUF_LEN 100

    #define MAX_FIELDS 20           // must be less than 127

    // Configure Liquid Crystal Display

        #define LCD_RS 8
        #define LCD_E 7
        #define LCD_D4 5            // It would have been nice to use the pro-mini A6 and A7 pins
        #define LCD_D5 4            // with the LCD unfortunately, these are I/O are designed for
        #define LCD_D6 A4           // analog input only.
        #define LCD_D7 A5

    // Tri_LED

        #define TRI_LED_R 9
        #define TRI_LED_B 10
        #define TRI_LED_G 11

    #define LED_PIN 13
    #define BUZ_PIN 2

    #define JOY_PUSH_PIN 12
    #define JOY_VERT A0
    #define JOY_HORZ A1
    #define JOY_PRES 0x00

#endif
