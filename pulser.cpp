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
    // write high pulse
    mux_high.set(strip);


    if (strip==0) {
        if (side==LEFT) {         // left side
            mux_prev.set(0x1);    // medium pulse to prev
            mux_low.set(1); // low pulse to next

            mux_med.set(15); // displace
            mux_next.set(3); // disable
        }
        else if (side==RIGHT) {   // right side
            mux_prev.set(0x2);    // low pulse to prev
            mux_med.set(strip+1); // med pulse to next

            mux_low.set(15); // displace
            mux_next.set(3); // disable
        }
    }

    else if (strip==15) {
        if (side==LEFT) {         // left side
            mux_next.set(0x2);    // low pulse to next
            mux_med.set(strip-1); // med pulse to n-1

            mux_low.set(0);  // displace
            mux_prev.set(3); // disable
        }
        else if (side==RIGHT) {   // right side
            mux_next.set(0x1);    // med pulse to next
            mux_low.set(strip-1); // med pulse to next

            mux_med.set(0);  // displace
            mux_prev.set(3); // disable
        }
    }

    else {
        mux_prev.set(3); // disable
        mux_next.set(3); // disable

        if (side==LEFT) {

        //SerialUSB.print("set mux left strip ");
        //SerialUSB.print(strip);
        //SerialUSB.print("\r\n");

        mux_med.set(strip-1);
        mux_low.set(strip+1);

        }
        else {
        //SerialUSB.print("set mux right strip ");
        //SerialUSB.print(strip);
        //SerialUSB.print("\r\n");
        mux_med.set(strip+1);
        mux_low.set(strip-1);
        }
    }

    //fpga.writeAddress(mux_high.adr()); // all mux are on the same address
    fpga.writeAddress(adr_mux_ctrl); // all mux are on the same address
}
