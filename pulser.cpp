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

//    int high;
//    int med;
//    int low;
//
//    high = strip;
//
//    if (strip==0) {
//        if (side==LEFT) {
//            med (prev);
//            low (strip+1);
//        }
//        else {
//            low (prev);
//            med (strip+1);
//        }
//    }
//
//    else if (strip=15) {
//        if (side==LEFT) {
//            med (strip-1);
//            low (next);
//        }
//        else {
//            low (strip-1);
//            med (next);
//        }
//    }
//
//    else {
//        if (side==LEFT) {
//            med (strip-1);
//            low (strip+1);
//        }
//        else {
//            med (strip+1);
//            low (strip-1);
//        }
//    }


    active_strip_mask.set(0x1 << strip);
    fpga.writeAddress(active_strip_mask.getAdr());

    uint32_t halfstrip_mask = 0x1 << (strip*2+side);

    halfstrips_expect_lsbs.set((halfstrip_mask>> 0) & 0xffff);
    halfstrips_expect_msbs.set((halfstrip_mask>>16) & 0xffff);

    fpga.writeAddress(halfstrips_expect_msbs.getAdr());
    fpga.writeAddress(halfstrips_expect_lsbs.getAdr());

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
