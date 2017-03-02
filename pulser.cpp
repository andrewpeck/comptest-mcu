#include "pulser.h"
#include <cstdio>
#include <cassert>

#include "fpga.h"
#include "fpga_fields.h"
#include "global_objects.h"

void Pulser::setStrip (int strip, int side)
{
    if (!side==LEFT || !side==RIGHT)
        return;
    if (strip<0 || strip>15)
        return;

    active_halfstrip.set(strip*2+side);
    halfstrip_mask_en.set(1);
    fpga.writeAddress(active_halfstrip.getAdr());

    writeMux(strip, side);
}


void Pulser::writeMux (int strip, int side)
{
    // write high pulse
    mux_high.set(strip);


    if (strip==0) {
        if (side==LEFT) { // left side
            mux_prev.set(0x2); // medium pulse to prev
            mux_low.set(strip+1); // low pulse to next

            mux_med.set(16); // displace
            mux_next.set(4); // disable
        }
        else if (side==RIGHT) { // right side
            mux_prev.set(0x3); // low pulse to prev
            mux_med.set(strip+1); // med pulse to next

            mux_low.set(16); // displace
            mux_next.set(4); // disable
        }
    }
    else if (strip==15) {
        if (side==LEFT) { // left side
            mux_next.set(0x3); // low pulse to next
            mux_med.set(strip-1); // med pulse to n-1

            mux_low.set(0); // displace
            mux_prev.set(4); // disable
        }
        else if (side==RIGHT) {           // right side
            mux_next.set(0x2);        // med pulse to next
            mux_low.set(strip-1); // med pulse to next

            mux_med.set(0); // displace
            mux_prev.set(4); // disable
        }
    }

    else {
        mux_prev.set(4); // disable
        mux_next.set(4); // disable

        if (side==LEFT) {
        mux_med.set(strip-1);
        mux_low.set(strip+1);
        }
        else {
        mux_med.set(strip+1);
        mux_low.set(strip-1);
        }
    }

    fpga.writeAddress(mux_low.getAdr());
    fpga.writeAddress(mux_med.getAdr());
    fpga.writeAddress(mux_high.getAdr());
    fpga.writeAddress(mux_prev.getAdr());
    fpga.writeAddress(mux_next.getAdr());
}
