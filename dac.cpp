#include "dac.h"
#include "spi.h"
#include <cassert>
#include "pinmodes.h"

void AD5551::write (uint16_t dac_counts)
{
    // assert (dac_counts <  (1<<14));

    dac_counts &= 0x3fff; // 14 bit DAC--- upper pad the 2 msbs with zeroes

    writeDirectPort0(_en_pin, LOW);
    spin::transfer16(dac_counts);
    writeDirectPort0(_en_pin, HIGH);
}


double AD5551::dacCountsToVolts (int dac_counts)
{
    return (dac_counts / static_cast<double>(0x3FF) * _vref);
}

void AD5551::writeVoltage (double voltage)
{
    int dac_counts = static_cast<int>((0x3FF)*(voltage/_vref));
    write(dac_counts);
}

void CDAC::writeThreshold (float millivolts)
{
    //assert (millivolts>=0);

    int dac_counts = (_vref*1000-millivolts)*((1<<14)-1)/_vref;
    write(dac_counts);
}
