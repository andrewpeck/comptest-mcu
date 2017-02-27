#include "SPI.h"

namespace spin {

    uint8_t transfer (uint8_t data) {
        return SPI.transfer(data);
    }

    uint16_t transfer16 (uint16_t data_wr) {
        uint16_t data_rd  = SPI.transfer ((data_wr >> 8) & 0xff)  << 0;
        data_rd |= SPI.transfer ((data_wr >> 0) & 0xff)  << 8;
    }

    void transfer24 (uint32_t data) {
        SPI.transfer ((data >> 16) & 0xff);
        SPI.transfer ((data >>  8) & 0xff);
        SPI.transfer ((data >>  0) & 0xff);
    }

    void transfer32 (uint32_t data) {
        SPI.transfer ((data >> 16) & 0xff);
        SPI.transfer ((data >>  8) & 0xff);
        SPI.transfer ((data >>  0) & 0xff);
    }

}
