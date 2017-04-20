#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <stdint.h>

/* Default DAQMB constructor sets comp_mode=2, comp_timing=1 */
// static const auto PKMODE = Comparator::PKMODE0;
// static const auto PKTIME = Comparator::PKTIME25;

static const uint8_t BX_DELAY       = 8;  // in reality, measure ~22bx between pulse and receiving decoded triads.   Open window at 16, look for 32 bx should be ok.
static const uint8_t PULSE_WIDTH    = 0x1;
static const uint8_t PKMODE         = 2; // def=2; 2 bits
static const uint8_t PKTIME         = 7; // def=1; 3 bits
static const uint8_t TRIAD_PERSIST  = 2;
static const uint8_t TRIAD_PERSIST1 = 0;
static const uint8_t COMPIN_INJECT  = 0;
static const uint8_t DDD_DELAY      = 0;
static const float   THRESH_VOLTAGE = 14; // millivolts
static const float   OFFSET_VOLTAGE = 5; // millivolts
static const uint16_t   RESTORE_CNT = 2048;

enum {test_offset, test_thresh, test_currents};


const bool split_packets = true;

#endif /* DEFAULTS_H */
