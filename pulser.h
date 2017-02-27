#ifndef PULSER_H
#define PULSER_H

#include <stdint.h>

enum Amplitude {PULSE_HIGH, PULSE_MED, PULSE_LOW, PULSE_OFF};
enum Side      {LEFT, RIGHT};

class Pulser {
    public:
        void setStrip (int strip, int side);
    private:

        void writeMux(int strip, int side);
};

#endif /* PULSER_H */
