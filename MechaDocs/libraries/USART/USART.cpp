// AVR Libc includes
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>

    #include "USART.h"

/* Set the size of the circular buffer */

    #define circ_buf_len 128                // must be a power of 2 (PO2)
    #define modulo_mask 0b01111111          // must be the binary representation of circ_buf_len - 1


// Private variables

    static volatile uint8_t circ_buf[circ_buf_len];
    static volatile uint8_t circ_buf_head = 0;
    static volatile uint8_t circ_buf_tail = 0;

    static volatile char line_terminator = 0x0A;    // default is ASCII Line Feed

 /** USART_handle_ISR
 * @brief This Interrupt Service Routine is called when a new character is received by the USART.
 * As quickly as possible, the AVR transfers the character to a circular buffer.  The main loop code
 * then retrieves data from this buffer.  Observe that this mechanism allows data to be received at
 * a high rate of speed independent of the main loop.
 *
 * @todo Idealy the entire ISR would be located in this file.  Unfortunatly there is a error
 * "multiple definition of `vector_18".  Apparently Arduino detects when an ISR is in the main
 * sketch.  If you place it somewhere else it is missed and replaced with the Arduino handler.
 * This is the source of the multiple definitions error.  See discussion at:
 *
 *     http://forum.arduino.cc/index.php?topic=42153.0
 *
 * Note that the three variables used in this function are all global is scope.  This was done so
 * that this function could be included in the projects main page.
 *
 * @note The vector name is not "USART_RXC" as indicated in the data sheet.
 * Instead, the ATMega328p "iom328p.h" file identified the name as "USART_RX_vect".

 * @note From the ATMEL data sheet "When interrupt driven data reception is used, the receive
 * complete routine must read the received data from UDRn in order to clear the RXCn Flag, otherwise
 * a new interrupt will occur once the interrupt routine terminates.
 */
void USART_handle_ISR(void){
   circ_buf[circ_buf_head] = UDR0;
   circ_buf_head++;
   circ_buf_head &= modulo_mask;
}



/** USART_init
 *
 * @brief Configure the USART and configure the ISR to interrupt on receipt of a new char.
 *
 * The baud rate is calculated as:
 *
 *                  f_osc
 *      BAUD = -----------------
 *              16 * (UBRR + 1)
 *
 * of if your prefer,
 *
 *                 f_osc
 *      UBBR = --------------  - 1
 *               16 * BAUD
 *
 * @param f_clk - master clock frequency,  usually 16000000 for the Arduino
 *
 * @param baud_rate desired speed of the USART e.g., 19200L
 *

 *
 * @return void
 */
void USART_init(unsigned long f_clk, unsigned long baud_rate){

    #define desired_UBRR ((f_clk/(16UL * baud_rate)) - 1)

    cli();                                                        // Disable global
    UBRR0H = (uint8_t)(desired_UBRR >> 8);                        // Set the baud rate generator
    UBRR0L = (uint8_t)desired_UBRR;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);         // Enable the USART hardware as well as the interrupt flag 
    UCSR0C = (1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);        // 8 bits, 2 stop bit, no parity

    if(baud_rate == 115200){
        UCSR0A = (1 << U2X0);                                     // enable double speed operation for lower % error at high speed
        UBRR0L = 16;
    }

    sei();                                                        // Enable global
}

/** USART_set_terminator
 *
 * @brief allows the user to set the string line terminator.  This function is optional.  The user
 * may elect to use the default ASCII line feed terminator (0x0A)
 *
 * @param terminator set the desired line terminator.
 */
void USART_set_terminator(char terminator){
    line_terminator = terminator;
}



void USART_gets (char *P){

    while (circ_buf_tail != circ_buf_head){
        if (circ_buf[circ_buf_tail] == line_terminator){
            circ_buf_tail++;
            circ_buf_tail &= modulo_mask;
            break;
        }
        *P = circ_buf[circ_buf_tail];
        P++;
        circ_buf_tail++;
        circ_buf_tail &= modulo_mask;
    }
    *P = 0x00;                              // null terminate
}



uint8_t USART_is_string(void){

    uint8_t result = 0x00;                       // default answer
    uint8_t i = circ_buf_tail;                   // start looking at end

    if (circ_buf_tail != circ_buf_head){

        /* The next line of code requires a bit of explanation.  Recall that the next char
         * into the circular buffer is inserted at the location pointed to by HEAD.  As we
         * "peek" into the circular buffer we start at TAIL and work our way to HEAD.
         * However, we don't want to look at HEAD itself.  We want one less than HEAD.
         * An easy way to perform this calculation is to take advantage of modulo math.
         * Add one less than the size of the buffer and mask of the "overflow bit."
         */

        while (i != ((circ_buf_head + (circ_buf_len - 1)) & modulo_mask )){
            i++;
            i &= modulo_mask;
            if (circ_buf[i] == line_terminator){
                result = 0x01;
                break;
            }
        }
    }
    return result;
}



void USART_puts(char *D){

    do {
        UDR0 = *D;                              // send a byte
        while ( !( UCSR0A & (1 << UDRE0)) );    // wait before sending the next byte
        D++;
    }
    while(*D != 0x00);
}



void USART_puts_ROM(const char *D){

    do {
        UDR0 = *D;                              // send a byte
        while ( !( UCSR0A & (1 << UDRE0)) );    // wait before sending the next byte
        D++;
    }
    while(*D != 0x00);
}



/*
void USART_nb_puts_RAM(char *D){

    static uint_8 lockout;

    do {
        TXREG = *D;                        // send a byte
        while(!(TXSTA & 0b00000010));      // wait before sending the next byte
        D++;
        }
    while(*D != NULL);
}

*/