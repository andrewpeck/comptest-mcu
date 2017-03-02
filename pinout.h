#ifndef PINOUT_H
#define PINOUT_H

#include <sam.h>

#include <stdint.h>
#include <Arduino.h>

// map taken from variants.cpp

enum {
    // digital
    pa23 = 0,
    pa22 = 1,
    pa21 = 2,
    pa20 = 3,
    pa13 = 4,
    pa12 = 5,
    pa15 = 6,
    pb22 = 7,
    pb10 = 8,
    pb11 = 9,
    pb23 = 10,
    pa14 = 11,
    pb2  = 12,
    pa28 = 13,
    pa8  = 14,
    pa9  = 15,
    pa10 = 16,
    pa11 = 17,
    pa18 = 28,

    // analog
    pa2  = 0,
    pa3  = 0,
    pb8  = 0,
    pb9  = 0,
    pa4  = 0,
    pa5  = 0,
    pa6  = 0,
    pa7  = 0
};

enum {
    analog_1 = pa2,
    analog_2 = pa3,
    analog_3 = pb8,
    analog_4 = pb9,
    analog_5 = pa4,
    analog_6 = pa5,
    analog_7 = pa6,
    analog_8 = pa7,
};

// map taken from display board schematic; convert from io# to PA/BX
enum {
    v_ifamp_pin        = analog_1,
    v_iamp_pin         = analog_2,
    v_ioff_pin         = analog_3,
    v_ibias_pin        = analog_4,
    comp_sense_5v5_pin = analog_5,
    comp_sense_3v3_pin = analog_6,

    ddd_al_pin         = PORT_PB23,
    pdac_en_pin        = PORT_PA21,
    cdac_en_pin        = PORT_PA22,
    samd_io0           = PORT_PA15, // fire (output)
    samd_io1           = PORT_PA13, // ready (input)
    samd_io2           = PORT_PA12, // reset counters (output)
    samd_io3           = PORT_PB11, // fpga_select
    samd_io4           = PORT_PB10, // fpga wr_en
    samd_io5           = PORT_PA20
};

enum {
    fpga_fire_pin  = samd_io0, // pa15
    fpga_ready_pin = samd_io1, // pa13
    fpga_reset_pin = samd_io2, // pa12
    fpga_cs_pin    = samd_io3, // pb11
    fpga_wren_pin  = samd_io4 // pb10
};

#endif /* pinout.h */
