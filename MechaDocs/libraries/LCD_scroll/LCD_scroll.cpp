
/**
* @brief This function allows you to send long ROM based text strings to the LCD display. It
* automatically performs the scrolling operation.
*
* @param *in_str is a pointer to the string to be sent.
*
* @return void
*
* @note This function depends on the Arduino LCD library.
*
* @note The LCD has an address for each character displayed on the screen. It also has memory for
* characters that are to the right of the screen. These “unseen” locations may be revealed by
* commanding the LCD to scroll right.
*
* This function breaks the message into units of 16 characters.
*
* 1) Clear the LCD display
* 2) Load the first 16 characters in the memory location immediately to the right of the LCD
* 3) Scroll left 16 times with a delay between each step allowing the operator to view the message
* 4) Load the first 32 character of the message starting at the first visible LDC position (yes you
*    are resending the first 16 characters of the message)
* 5) Scroll left 32 times
* 6) Load the next 32 characters
* 7) Repeat steps 5 and 6 until complete message has been loaded
*/
void LCD_scroll_long_ROM_string(const char *in_str) {

    char buf[32];
    uint8_t str_length = strlen(in_str);
    uint8_t i, j;

    lcd.clear(); // Load the first 16 char in the LCD memory to the unseen positions to the right of the display
    lcd.setCursor(16, 0);
    snprintf(buf, 16, "%s", in_str); // fancy way of parsing 16 characters from string
    lcd.print(buf);

    for(j = 1; j < 16; j++){
        lcd.scrollDisplayLeft();
        delay(200);
    }

    for (i = 0; i < (str_length / 16) + 1; i++){
        lcd.clear();
        lcd.setCursor(0, 0);
        snprintf(buf, 32, "%s", in_str + 16 * i );
        lcd.print(buf);
        for(j = 1; j< 16; j++){
            lcd.scrollDisplayLeft();
            delay(200);
        }
    }
}
