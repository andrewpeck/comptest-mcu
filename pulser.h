#ifndef PULSER_H
#define PULSER_H

#include <stdint.h>
#include "defaults.h"

enum Amplitude {PULSE_HIGH, PULSE_MED, PULSE_LOW, PULSE_OFF};
enum Side      {LEFT=0, RIGHT=1};

class Pulser {
    public:
        void setStrip (uint8_t strip, uint8_t side, uint8_t scan=test_offset);
    private:

        void writeMux(uint8_t strip, uint8_t side);
        void writeMuxThresh (uint8_t strip, uint8_t side);
};

#endif /* PULSER_H */
