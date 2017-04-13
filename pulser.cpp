#include "pulser.h"
#include <cstdio>
#include <cassert>

#include "fpga.h"
#include "fpga_fields.h"
#include "global_objects.h"

void Pulser::setStrip (uint8_t strip, uint8_t side)
{
    active_halfstrip.set(uint16_t(strip)*2+side);
    halfstrip_mask_en.set(1);

    fpga.writeAddress(adr_active_strip_mask);

    mux_en.set(1);
    fpga.writeAddress(mux_en.adr());

    writeMux(strip, side);
}


void Pulser::writeMux (uint8_t strip, uint8_t side)
{

    uint8_t low=3;
    uint8_t med=2;
    uint8_t high=1;
    uint8_t off=3;

    // write high pulse
    mux_high.set(strip);

    //------------------------------------------------------------------------------------------------------------------
    // Left Edge
    //------------------------------------------------------------------------------------------------------------------

    if (strip==0) {

        if (side==LEFT) {         // left side

            mux_prev.set(med);    // medium pulse to prev
            mux_low.set(strip+1); // low pulse to next

            mux_next.set(off); // disable
            mux_med.set(15); // displace
        }

        else if (side==RIGHT) {   // right side
            mux_prev.set(low);    // low pulse to prev
            mux_med.set(strip+1); // med pulse to next

            mux_low.set(15); // displace
            mux_next.set(3); // disable
        }

    }

    //------------------------------------------------------------------------------------------------------------------
    // Right Edge
    //------------------------------------------------------------------------------------------------------------------

    else if (strip==15) {

        if (side==LEFT) {
            mux_next.set(low);    // low pulse to next
            mux_med.set(strip-1); // med pulse to n-1

            mux_low.set(0);  // displace
            mux_prev.set(off); // disable
        }

        else if (side==RIGHT) {   // right side
            mux_next.set(med);    // med pulse to next
            mux_low.set(strip-1); // low pulse to strip-1

            mux_med.set(0);  // displace
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

    //fpga.writeAddress(mux_high.adr()); // all mux are on the same address
    fpga.writeAddress(adr_mux_ctrl); // all mux are on the same address
}
