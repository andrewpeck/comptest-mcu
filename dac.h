#ifndef DAC_H
#define DAC_H

#include <stdint.h>

#include "pinout.h"

class AD5551 {
    public:

        AD5551 (uint32_t en_pin, float vref)
        {
            _vref = vref;
            _en_pin = en_pin;
        };

        void write          (uint16_t dac_counts);
        void writeVoltage   (double voltage);

    protected:

        float  _vref;

    private:

        double dacCountsToVolts (int dac_counts);
        uint32_t _en_pin;
};

class PDAC : public AD5551 {
    public:
        PDAC () : AD5551 (pdac_en_pin, 3.3) {};
};

class CDAC : public AD5551 {
    public:
        void writeThreshold (float millivolts);
        CDAC () : AD5551 (cdac_en_pin, 3.3) {}; // FIXME: need the right pin numbers
};

#endif
