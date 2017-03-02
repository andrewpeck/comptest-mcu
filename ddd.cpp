#include "ddd.h"
#include <cassert>
#include <Arduino.h>

#include "spi.h"
#include "global_objects.h"

DDD::DDD (int sclk_pin, int mosi_pin, int latch_pin)
{
    _sclk_pin  = sclk_pin;
    _mosi_pin  = mosi_pin;
    _latch_pin = latch_pin;
}

void DDD::setDelay (int ns_delay)
{
    assert (ns_delay>=0);
    assert (ns_delay<=120);

    /* ddd chip steps in units of 2ns */
    int ddd_delay = ns_delay / 2;

    struct ddd_config dddconf;
    dddconf.ch1enable = 1;
    dddconf.ch2enable = 1;
    dddconf.ch3enable = 1;
    dddconf.ch4enable = 1;

    if (ddd_delay <= 15) {
        dddconf.ch1delay = ddd_delay;
        dddconf.ch2delay = 0;
        dddconf.ch3delay = 0;
        dddconf.ch4delay = 0;
    }
    else if (ddd_delay <= 30) {
        dddconf.ch1delay = 15;
        dddconf.ch2delay = ddd_delay-15;
        dddconf.ch3delay = 0;
        dddconf.ch4delay = 0;
    }
    else if (ddd_delay <= 45) {
        dddconf.ch1delay = 15;
        dddconf.ch2delay = 15;
        dddconf.ch3delay = ddd_delay-30;
        dddconf.ch4delay = 0;
    }
    else if (ddd_delay <= 60) {
        dddconf.ch1delay = 15;
        dddconf.ch2delay = 15;
        dddconf.ch3delay = 15;
        dddconf.ch4delay = ddd_delay-45;
    }

    writeDelay (dddconf);
}

void DDD::writeDelay (ddd_config config)
{
    //printf("ddd1delay: %i\n", config.ch1delay);
    //printf("ddd2delay: %i\n", config.ch2delay);
    //printf("ddd3delay: %i\n", config.ch3delay);
    //printf("ddd4delay: %i\n", config.ch4delay);

    uint32_t data = 0;

    data |= (0x1 & config.ch4enable) << 19;
    data |= (0x1 & config.ch3enable) << 18;
    data |= (0x1 & config.ch2enable) << 17;
    data |= (0x1 & config.ch1enable) << 16;
    data |= (0xF & config.ch1delay ) << 12;
    data |= (0xF & config.ch2delay ) << 8;
    data |= (0xF & config.ch3delay ) << 4;
    data |= (0xF & config.ch4delay ) << 0;


    bool useSPI = 1;

    if (useSPI) {
        setDddAl(0);
        spin::transfer24 (data);
        setDddAl(1);
    }

//     {
//         /* annoying 20 bit spi... just bit bang for now? */
//         /* later can try to see if we can clock in 4 dummy bits and use SPI */
//
//         /* make sure to end the spi bus */
//         SPI.end();
//         digitalWrite(_sclk_pin,  LOW);
//         digitalWrite(_mosi_pin,  LOW);
//         digitalWrite(_latch_pin, HIGH);
//
//         for (int iclk=0; iclk<20; iclk++) {
//             digitalWrite(_sclk_pin, LOW);
//
//             int databit = (0x1 & (data >> (19-iclk))) ? MOSI : 0; // Data
//
//             digitalWrite(_mosi_pin, databit);
//
//             digitalWrite(_sclk_pin, HIGH);
//         }
//
//         digitalWrite(_sclk_pin,  LOW);
//
//         // need at least 10 ns here
//         digitalWrite(_latch_pin, LOW);
//         delayMicroseconds(1);
//         digitalWrite(_latch_pin, HIGH);
//
//         /* renable SPI when we are done */
//         SPI.begin();
//     }
}
