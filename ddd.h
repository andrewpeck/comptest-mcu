#ifndef DDD_H
#define DDD_H

#include <stdint.h>

class DDD {

    public:

     DDD (int sclk_pin, int mosi_pin, int latch_pin) ;

    void setDelay (int ns_delay);

    private:

    struct ddd_config {
        bool ch1enable;
        int ch1delay;

        bool ch2enable;
        int ch2delay;

        bool ch3enable;
        int ch3delay;

        bool ch4enable;
        int ch4delay;
    };

    void writeDelay (ddd_config config);

    uint8_t _sclk_pin; // TODO: add defaults...
    uint8_t _mosi_pin;
    uint8_t _latch_pin;

};
#endif
