#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

class Controller {

    public:
        void initialize();
        void offsetScan (uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data);
        void threshScan (uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data);
        void pulse      (uint16_t dac_value, uint16_t num_pulses, uint16_t num_loops, uint8_t strip, uint8_t side);
        void scan       (uint16_t dac_value);

    private:
        void genericScan(bool test_type, uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data);
};
#endif /* CONTROLLER_H */
