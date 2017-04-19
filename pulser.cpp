#include "pulser.h"
#include <cstdio>
#include <cassert>

#include "fpga.h"
#include "fpga_fields.h"
#include "global_objects.h"
#include "defaults.h"

void Pulser::setStrip (uint8_t strip, uint8_t side, uint8_t scan)
{

    active_halfstrip.set(uint16_t(strip)*2+side);
    halfstrip_mask_en.set(1);

    fpga.writeAddress(adr_active_strip_mask);

    mux_en.set(1);
    fpga.writeAddress(mux_en.adr());

    if (scan==test_thresh)
        writeMuxThresh(strip, side);
    else
        writeMux(strip, side);
}


void Pulser::writeMuxThresh (uint8_t strip, uint8_t side)
{
    uint8_t low  = 2;
    uint8_t med  = 1;
    uint8_t high = 0;
    uint8_t off  = 3;

    // write high pulse
    mux_high.set(strip);

    mux_med.set(strip+2);
    mux_low.set(strip-2);

    mux_prev.set(off);
    mux_next.set(off);

    fpga.writeAddress(adr_mux_ctrl); // all mux are on the same address
}

void Pulser::writeMux (uint8_t strip, uint8_t side)
{

    uint8_t low  = 2;
    uint8_t med  = 1;
    uint8_t high = 0;
    uint8_t off  = 3;

    // write high pulse
    mux_high.set(strip);

    //------------------------------------------------------------------------------------------------------------------
    // Left Edge
    //------------------------------------------------------------------------------------------------------------------

    if (strip==0) {

        if (side==LEFT) {         // left side

            mux_low.set(strip+1); // low pulse to 1
            mux_prev.set(med);    // medium pulse to prev

            mux_next.set(off); // disable
            mux_med.set(15);   // displace

        }

        else if (side==RIGHT) {   // right side

            mux_prev.set(low); // low pulse to prev
            mux_med.set(1);    // med pulse to 1

            mux_low.set(15);   // displace
            mux_next.set(off); // disable

        }

    }

    //------------------------------------------------------------------------------------------------------------------
    // Right Edge
    //------------------------------------------------------------------------------------------------------------------

    else if (strip==15) {

        if (side==LEFT) {
            mux_next.set(low); // low pulse to next
            mux_med.set(14);   // med pulse to 14

            mux_low.set(0);    // displace to other side of the chip
            mux_prev.set(off); // disable
        }

        else if (side==RIGHT) {   // right side

            mux_next.set(med); // med pulse to next
            mux_low.set(14);   // low pulse to 14

            mux_med.set(0);    // displace
            mux_prev.set(off); // disable
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    // Standard Strips
    //------------------------------------------------------------------------------------------------------------------

    else {
        mux_prev.set(3); // disable
        mux_next.set(3); // disable

        if (side==LEFT) {
            mux_med.set(strip-1);
            mux_low.set(strip+1);
        }
        else {
            mux_med.set(strip+1);
            mux_low.set(strip-1);
        }
    }

    fpga.writeAddress(adr_mux_ctrl); // all mux are on the same address
}
