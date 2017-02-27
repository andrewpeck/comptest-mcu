#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <SPI.h>

namespace spin {
    extern uint8_t transfer (uint8_t data);
    extern uint16_t transfer16 (uint16_t data_wr);
    extern void transfer24 (uint32_t data);
    extern void transfer32 (uint32_t data);
}

#endif /* SPI_H */
