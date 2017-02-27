#ifndef DAC_H
#define DAC_H

#include <stdint.h>

class AD5551 {
    public:

        AD5551 (uint8_t en_pin=0, float vref=3.6020f) : _vref (vref), _en_pin (en_pin) {};

        void write          (uint16_t dac_counts);
        void writeVoltage   (double voltage);

    protected:
        float  _vref;

    private:

        double dacCountsToVolts (int dac_counts);
        uint8_t _en_pin;
};

class PDAC : public AD5551 {
    public:
        PDAC (uint8_t en_pin=0, float vref=3.3000f) : AD5551 (en_pin, vref) {};
};

class CDAC : public AD5551 {
    public:
        void writeThreshold (float millivolts);
        CDAC (uint8_t en_pin=0, float vref=3.6020f) : AD5551 (en_pin, vref) {}; // FIXME: need the right pin numbers
};

#endif
