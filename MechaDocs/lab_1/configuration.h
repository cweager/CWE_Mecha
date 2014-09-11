
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
