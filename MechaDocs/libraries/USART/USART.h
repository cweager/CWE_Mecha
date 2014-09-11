#ifndef _USART_H

    #define _USART_H
    #include <stdint.h>

    void USART_handle_ISR(void);
    void USART_init(unsigned long f_clk, unsigned long baud_rate);

    void USART_set_terminator(char terminator);

    void USART_gets(char *P);
    void USART_puts(char *D);

    void USART_puts_ROM(const char *D);

    uint8_t USART_is_string(void);

#endif
