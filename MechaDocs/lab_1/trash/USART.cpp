// AVR Libc includes
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>

    #include "configuration.h"
    #include "USART.h"

// Private variables

    static volatile uint8_t ISR_main_link_circ_bufer[32];   // FIXME bad form to hard code
    static volatile uint8_t circ_buf_head = 0;
    static volatile uint8_t circ_buf_tail = 0;

 /** USART_handle_ISR
 * @brief This Interrupt Service Routine is called when a new character is received by the USART.
 * As quickly as possible, the AVR transfers the character to
 * a circular buffer.  The main loop code then retrieves data from this buffer.
 * Observe that this mechanism allows data to be received at a high rate of speed
 * independent of the main loop.
 * 
 * @todo This function should be in the USART.c file but there is a error 
 * "multiple definition of `  vector_18".  Apparently Arduino detects when an ISR
 * is in the main sketch.  If you place it somewhere else it is missed and replaced 
 * with the Arduino handler.  This is the source of the multiple definitions
 * error -  * see discussion @ http://forum.arduino.cc/index.php?topic=42153.0
 * Note that the three variables used in this function are all global is scope.
 * This was done so that this function could be included in the projects main page.
 * 
 * @note The vector name is not "USART_RXC" as indicated in the data sheet.
 * Instead, the ATMega328p "iom328p.h" file identified the name as "USART_RX_vect".

 * @note From the ATMEL data sheet "When interrupt driven data reception is used, the
 * receive complete routine must read the received data from UDRn in order to
 * clear the RXCn Flag, otherwise a new interrupt will occur once the interrupt
 * routine terminates.
 *
 */
void USART_handle_ISR(void){
   ISR_main_link_circ_bufer[circ_buf_head] = UDR0;
   circ_buf_head++;
   circ_buf_head &= 0b00011111;                             // FIXME bad form to hard code
}



/* USART_init ()
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
 * @param f_clk - not exactly a parameter but it must be defined preferably near 
 * top of main e.g., #define f_clk 16000000L
 *
 * @param baud_rate - not exactly a parameter but it must be defined preferably near
 *  top of main e.g., #define baud_rate 19200L
 *
 * @return void
 */
void USART_init(){

    #define desired_UBRR ((f_clk/(16UL * baud_rate)) - 1)
    cli();                                                        // Disable global
    UBRR0H = (unsigned char)(desired_UBRR >> 8);                  // Set the baud rate generator
    UBRR0L = (unsigned char)desired_UBRR;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);         // Enable the USART hardware as well as the interrupt flag 
    UCSR0C = (1<<USBS0) | (1 << UCSZ01) | (1 << UCSZ00);          // 8 bits, 2 stop bit, no parity
    sei();                                                        // Enable global
}



void USART_gets (char *P){

    uint8_t i = circ_buf_tail;                  // start loop at tail and work towards buffer's head
    char temp;

    if (circ_buf_head == circ_buf_tail){        // shouldn't ever get here - better safe than sorry...
        *P = 0x00;
    }
    else{
        while (i != circ_buf_head){
            temp = ISR_main_link_circ_bufer[i];
            *P = temp;
            i++;
            i = i & 0b00011111;                              // FIXME bad form to hard code
            P++;
            if (temp == line_terminator){
                break;
            }
        }
        P--;                                    // kill the line terminator
        *P = 0x00;                              // null terminate
        circ_buf_tail = i;
    }
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



uint8_t USART_is_string(void){

    uint8_t result = 0x00;                       // default answer
    uint8_t i = circ_buf_tail;                   // start looking at end

    if (circ_buf_head != circ_buf_tail){

        /* The next line of code requires a bit of explanation.  Recall that the next char 
         * into the circular buffer is inserted at the location pointed to by HEAD.  As we 
         * "peek" into the circular buffer we start at TAIL and work our way to HEAD.  
         * However, we don't want to look at HEAD itself.  We want one less than HEAD.  An 
         * easy way to perform this calculation is to take advantage of modulo math.  Simply 
         * add 31 to the HEAD and mask off the overflow bit.
         */

        while (i != ((circ_buf_head + 31) & 0b00011111 )){           // FIXME bad form to hard code
            i ++;                               // mod 32
            i = i & 0b00011111;
            if (ISR_main_link_circ_bufer[i] == line_terminator){
                result = 0x01;
                break;
            }
        }
    }
    return result; 
}




/*
 * The following functions are taken directly from the Atmel data sheet document #8271G–AVR–02/2013
 */
 //   void USART_TX( unsigned char data ){
 //       while ( !( UCSR0A & (1 << UDRE0)) );        // Wait for empty transmit buffer 
 //       UDR0 = data;                                // Put data into buffer, sends the data 
 //   }


 //   unsigned char USART_RX( void ){
 //   // AVR has a two level FIFO..
 //       while ( !(UCSR0A & (1 << RXC0)) );          // Wait for data to be received 
 //       return UDR0;                                // Get and return received data from buffer
 //   }



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
