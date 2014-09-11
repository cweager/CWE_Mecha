#ifndef _SPI_H

    #define _SPI_H

    #include <stdint.h>

   void SPI_init(void);

   // void SPI_close(void);

    void SPI_transfer(uint8_t N, uint8_t *SPI_tx_buf, uint8_t *SPI_rx_buf);

#endif
