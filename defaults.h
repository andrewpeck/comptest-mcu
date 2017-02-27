#ifndef DEFAULTS_H
#define DEFAULTS_H 


/* Default DAQMB constructor sets comp_mode=2, comp_timing=1 */
// static const auto PKMODE = Comparator::PKMODE0;
// static const auto PKTIME = Comparator::PKTIME25;

static const uint8_t BX_DELAY = 2;
static const uint8_t PULSE_WIDTH = 1;
static const uint8_t PKMODE = 2;
static const uint8_t PKTIME = 1;
static const uint8_t TRIAD_PERSIST = 4;
static const uint8_t TRIAD_PERSIST1 = 0;
static const uint8_t COMPIN_INJECT = 0;
static const uint8_t DDD_DELAY = 0;
static const float   THRESH_VOLTAGE = 0.46f;

enum {test_offset, test_thresh, test_currents};

#endif /* DEFAULTS_H */
