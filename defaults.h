#ifndef DEFAULTS_H
#define DEFAULTS_H


/* Default DAQMB constructor sets comp_mode=2, comp_timing=1 */
// static const auto PKMODE = Comparator::PKMODE0;
// static const auto PKTIME = Comparator::PKTIME25;

static const uint8_t BX_DELAY       = 0x1;  // in reality, measure ~22bx between pulse and receiving decoded triads.   Open window at 16, look for 32 bx should be ok.
static const uint8_t PULSE_WIDTH    = 0x4;
static const uint8_t PKMODE         = 0; // 2
static const uint8_t PKTIME         = 1; // 1
static const uint8_t TRIAD_PERSIST  = 2;
static const uint8_t TRIAD_PERSIST1 = 0;
static const uint8_t COMPIN_INJECT  = 0;
static const uint8_t DDD_DELAY      = 0;
static const float   THRESH_VOLTAGE = 0.46f;
static const uint16_t   RESTORE_CNT = 4095;

enum {test_offset, test_thresh, test_currents};

#endif /* DEFAULTS_H */
