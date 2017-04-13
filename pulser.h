#ifndef PULSER_H
#define PULSER_H

#include <stdint.h>

enum Amplitude {PULSE_HIGH, PULSE_MED, PULSE_LOW, PULSE_OFF};
enum Side      {LEFT=0, RIGHT=1};

class Pulser {
    public:
        void setStrip (uint8_t strip, uint8_t side);
    private:

        void writeMux(uint8_t strip, uint8_t side);
};

#endif /* PULSER_H */
