#define NOMINMAX

#include "controller.h"
#include "dac.h"
#include "ddd.h"
#include "global_objects.h"
#include "fpga.h"
#include "pinout.h"
#include "fpga_fields.h"
#include "defaults.h"
#include "erflut.h"

// TODO: parameterize this with some configuration that can be accessed from s/w ?
void Controller::initialize()
{

    //------------------------------------------------------------------------------------------------------------------
    // Configure SPI Peripherals
    //------------------------------------------------------------------------------------------------------------------

    bool use_ddd=0;

    if (use_ddd) {
        ddd.setDelay(DDD_DELAY);
    }

//    cdac.write(0);
//    pdac.write(0);

    //------------------------------------------------------------------------------------------------------------------
    // Default FPGA Configuration
    //------------------------------------------------------------------------------------------------------------------

    fpga.readAll();

    bx_delay.set       (BX_DELAY);
    pulse_width.set    (PULSE_WIDTH);
    pkmode.set         (PKMODE);

    uint8_t time_corrected = 0;

    time_corrected |= (0x1&(PKTIME>>2))<<0; // take 0th bit from  2nd bit
    time_corrected |= (0x1&(PKTIME>>0))<<2; // take 2nd bit from 0th bit
    time_corrected |= (0x1&(PKTIME>>1))<<1; // take 1st bit from 1st bit

    pktime.set         (time_corrected);

    triad_persist.set  (TRIAD_PERSIST);
    triad_persist1.set (TRIAD_PERSIST1);
    compin_inject.set  (COMPIN_INJECT);
    restore_cnt.set    (RESTORE_CNT);
    lctrst.set         (0);

    fpga.resetCounters();

    fire_pulse.set(0);
    offset_errcnt_rst.set(0);
    compout_errcnt_rst.set(0);
    thresholds_errcnt_rst.set(0);
    compout_expect.set(0);

    fpga.writeAddress(fire_pulse.adr());
    fpga.writeAddress(bx_delay.adr());
    fpga.writeAddress(pulse_width.adr());
    fpga.writeAddress(pkmode.adr());
    fpga.writeAddress(pktime.adr());
    fpga.writeAddress(triad_persist.adr());
    fpga.writeAddress(triad_persist1.adr());
    fpga.writeAddress(compin_inject.adr());
    fpga.writeAddress(lctrst.adr());
    fpga.writeAddress(14);


}

void Controller::scan (uint16_t dac_value) {
    for (int istrip=0; istrip<16; istrip++) {
        if (istrip%2==0)
            SerialUSB.print("----------------------------------------\r\n");
    for (int iside=0; iside<2; iside++) {
        pulse (dac_value, 1, 1, istrip, iside);
    }
    }
}

void Controller::scanPeakTiming (uint16_t dac_value, uint16_t num_pulses, uint8_t strip, uint8_t side) {

    float mean0 = 0;

    // loop over peaktime
    for (uint8_t time=0; time<8; time++) {
        uint8_t time_corrected=0;

        // reverse the bit order here because I screwed up the PCB and put the wires in the wrong place
        // probably better to do this in the f/w ?

        time_corrected |= (0x1&(time>>2))<<0; // take 0th bit from  2nd bit
        time_corrected |= (0x1&(time>>0))<<2; // take 2nd bit from 0th bit
        time_corrected |= (0x1&(time>>1))<<1; // take 1st bit from 1st bit

        pktime.set         (time_corrected);
        pkmode.set         (0);

        fpga.writeAddress  (pktime.adr());

        cdac.writeThreshold(THRESH_VOLTAGE); //

        fire_num_pulses.set(1);
        fpga.writeAddress(fire_num_pulses.adr());

        pdac.write(dac_value);

        strip&=0xf;
        side&=0x1;

        pulser.setStrip(strip, side);

        delayMicroseconds(10);

        SerialUSB.print ("TIME::");
        SerialUSB.print (" pktime=");
        SerialUSB.print (time);
        SerialUSB.print (" ");

        for (int i=0; i<num_pulses; i++) {

            fpga.fire();
            while (!fpga.isReady()) ;

            fpga.readAddress(response_time.adr());
            uint8_t response = response_time.get();

            SerialUSB.print (response);
            SerialUSB.print ("  ");
        }

        SerialUSB.print ("\r\n");
    }
}

// void Controller::restore_scan (uint16_t dac_value=1000, uint8_t strip=1, uint8_t side=0) {
//
//     for (uint16_t  restore_val=0; restore_val<16; restore_val++) {
//
//             pulse (dac_value, 10, 1, istrip, iside);
//
//     }
//
// }

void Controller::scanMode (uint16_t dac_value, uint16_t num_pulses, uint8_t strip, uint8_t side) {

      uint16_t errors_hs    [4][8];
      uint16_t errors_strip [4][8];

      for (uint8_t mode=0; mode<4; mode++) {
          for (uint8_t time=0; time<8; time++) {

              pkmode.set         (mode);

              uint8_t time_corrected = 0;

              time_corrected |= (0x1&(time>>2))<<0; // take 0th bit from  2nd bit
              time_corrected |= (0x1&(time>>0))<<2; // take 2nd bit from 0th bit
              time_corrected |= (0x1&(time>>1))<<1; // take 1st bit from 1st bit

              pktime.set         (time_corrected);

              fpga.writeAddress(pkmode.adr());

              fpga.resetCounters();

              while (!fpga.isReady()) ;

              //initialize();

              cdac.writeThreshold(THRESH_VOLTAGE); //

              fire_num_pulses.set(num_pulses);
              fpga.writeAddress(fire_num_pulses.adr());

              pdac.write(dac_value);

              strip&=0xf;
              side&=0x1;

              pulser.setStrip(strip, side);

              delayMicroseconds(10);


              fpga.fire();
              while (!fpga.isReady()) ;

              fpga.readAddress(adr_halfstrips);
              fpga.readAddress(adr_halfstrips2);
              fpga.readAddress(adr_halfstrips2);

              fpga.readAddress(adr_offsets_errcnt);
              fpga.readAddress(adr_thresholds_errcnt);

              uint32_t halfstrips = (halfstrips_msbs.get() << 16) | halfstrips_lsbs.get();

              for (int i=0; i<32; i++) {
                  if (i%4==0)
                      SerialUSB.print("|");

                  if (i==(strip*2 + side)) {
                      if  ((halfstrips >> i) & 0x1)
                          SerialUSB.print("g"); // print this if we see a hit in the right spot
                      else
                          SerialUSB.print(" ");
                  }
                  else {
                      if  ((halfstrips >> i) & 0x1)
                          SerialUSB.print("-"); // print this if we see a hit in the wrong spot
                      else
                          SerialUSB.print(" ");
                  }
              }

              SerialUSB.print("|");
              SerialUSB.print("  (");
              SerialUSB.print(strip);
              SerialUSB.print(", ");
              SerialUSB.print(side);
              SerialUSB.print(")");

              SerialUSB.print("  (hs_error=");
              SerialUSB.print(offsets_errcnt.get());
              SerialUSB.print(", strip_errors=");
              SerialUSB.print(thresholds_errcnt.get());
              SerialUSB.print(")");
              SerialUSB.print ("\r\n");

              errors_hs    [mode][time]=offsets_errcnt.get();
              errors_strip [mode][time]=thresholds_errcnt.get();

          }}

      SerialUSB.print("|0  1|2  3|4  5|6  7|8  9|A  B|C  D|E  F|\r\n");

      char msg [240];

      sprintf(msg, "halfstrip_errors\r\n");
      SerialUSB.print(msg);
      sprintf(msg, "          0       1      2       3        4       5       6       7\r\n");
      SerialUSB.print(msg);
      for (uint8_t imode=0; imode<4; imode++) {
      sprintf(msg, "%1i       %3i     %3i    %3i     %3i      %3i     %3i     %3i     %3i\r\n", imode, errors_hs[imode][0], errors_hs[imode][1], errors_hs[imode][2], errors_hs[imode][3], errors_hs[imode][4], errors_hs[imode][5], errors_hs[imode][6], errors_hs[imode][7]);
      SerialUSB.print(msg);
      }

      sprintf(msg, "strip_errors\r\n");
      SerialUSB.print(msg);
      sprintf(msg, "          0       1      2       3        4       5       6       7\r\n");
      SerialUSB.print(msg);
      for (uint8_t imode=0; imode<4; imode++) {
      sprintf(msg, "%1i       %3i     %3i    %3i     %3i      %3i     %3i     %3i     %3i\r\n", imode, errors_strip[imode][0], errors_strip[imode][1], errors_strip[imode][2], errors_strip[imode][3], errors_strip[imode][4], errors_strip[imode][5], errors_strip[imode][6], errors_strip[imode][7]);
      SerialUSB.print(msg);
      }

}

void Controller::pulse(uint16_t dac_value, uint16_t num_pulses, uint16_t num_loops, uint8_t strip=1, uint8_t side=0) {

    uint16_t errors_hs    [4][8];
    uint16_t errors_strip [4][8];

    //-Compout----------------------------------------------------------------------------------------------------------
    if (strip==15)
        compout_expect.set(1);
    else
        compout_expect.set(0);
    fpga.writeAddress(adr_pulse_ctrl);

    fpga.resetCounters();

    while (!fpga.isReady()) ;

    //initialize();

    cdac.writeThreshold(THRESH_VOLTAGE); //

    fire_num_pulses.set(num_pulses);
    fpga.writeAddress(fire_num_pulses.adr());
    //
    // //           //    SerialUSB.print ("Setting DAC to: ");
    // //           //    SerialUSB.print (dac_value, HEX);
    // //           //    SerialUSB.print ("\r\n");
    pdac.write(dac_value);
    //
    // //            //   SerialUSB.print ("Setting mux to strip=");
    // //            //   SerialUSB.print (strip);
    // //            //   SerialUSB.print (" side=");
    // //            //   SerialUSB.print (side);
    // //            //   SerialUSB.print ("\r\n");
    //

    strip&=0xf;
    side&=0x1;

    pulser.setStrip(strip, side);
    //
    delayMicroseconds(10);
    //
    //
    for (uint16_t loop=0; loop<num_loops; loop++) {
        fpga.fire();
        while (!fpga.isReady()) ;
        //delayMicroseconds(4);
    }

    fpga.readAddress(adr_halfstrips);
    fpga.readAddress(adr_halfstrips2);
    fpga.readAddress(adr_halfstrips2);

    fpga.readAddress(adr_offsets_errcnt);
    fpga.readAddress(adr_thresholds_errcnt);
    fpga.readAddress(adr_compout_errcnt);

    uint32_t halfstrips = (halfstrips_msbs.get() << 16) | halfstrips_lsbs.get();

    for (int i=0; i<32; i++) {
        if (i%4==0)
                SerialUSB.print("|");

        if (i==(strip*2 + side)) {
            if  ((halfstrips >> i) & 0x1)
                SerialUSB.print("g"); // print this if we see a hit in the right spot
            else
                SerialUSB.print(" ");
        }
        else {
            if  ((halfstrips >> i) & 0x1)
                SerialUSB.print("-"); // print this if we see a hit in the wrong spot
            else
                SerialUSB.print(" ");
        }
    }
    SerialUSB.print("|");
    SerialUSB.print("  (");
    SerialUSB.print(strip);
    SerialUSB.print(", ");
    SerialUSB.print(side);
    SerialUSB.print(")");

    SerialUSB.print("  (hs_error=");
    SerialUSB.print(offsets_errcnt.get());
    SerialUSB.print(", strip_errors=");
    SerialUSB.print(thresholds_errcnt.get());
    SerialUSB.print(", compout_errors=");
    SerialUSB.print(compout_errcnt.get());
    SerialUSB.print(")");
    SerialUSB.print ("\r\n");
}

//----------------------------------------------------------------------------------------------------------------------
// Want to set a threshold, then look for **any** response on that strip (halfstrip doesn't matter really)
//----------------------------------------------------------------------------------------------------------------------

void Controller::genericScan(bool test_type, uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data)
{

    /* write the threshold physically to the comparator DAC */

    if (test_type==test_thresh) {
        cdac.writeThreshold(THRESH_VOLTAGE); //
    }
    else {
        cdac.writeThreshold(OFFSET_VOLTAGE); //
    }

    delayMicroseconds(100);

    /* wait for pulser ready... poll the board until it is OK. */
     while (!fpga.isReady()) ;

    // don't ask where this magic number came from...
    // its the number of different settings in a for loop that starts at 1 and skips values
    // it was mostly by trial and error but seems to be reliable (tested by python script)

    //static const int NUM_SETTINGS = max/step + 1 - (max % step <=1) - (max % step==0 && step==1);

    fire_num_pulses.set(num_pulses);
    fpga.writeAddress(fire_num_pulses.adr());

    uint16_t dac_value = dac_start;
    pdac.write(dac_value);
    delayMicroseconds(25);

    bitField *field = (test_type==test_thresh) ? &thresholds_errcnt : &offsets_errcnt;

    for (uint16_t index=0; index<1024; index++) {

        delayMicroseconds(1);

        fpga.resetCounters();

        fpga.fire();

        if (split_packets && index>0) {
            char msg [0];
            sprintf(msg, "%04X", data[index-1]);
            SerialUSB.print(msg);
        }

        while (!fpga.isReady()) ;

        //-DAC----------------------------------------------------------------------------------------------------------
        dac_value += dac_step;

        /* wait some delay for DAC to settle initially .. subsequent incremental changes can be faster */
        pdac.write(dac_value);


        //-Readout------------------------------------------------------------------------------------------------------

        fpga.readAddress (field->adr());

        // optionally just send an error function from RAM
        static const int useerf = 0;

        if (useerf)
            data[index] = erflut[index];
        else
            data[index] = field->data();
    }

    if (split_packets) {
        char msg [0];
        sprintf(msg, "%04X", data[1023]);
        SerialUSB.print(msg);
    }
}

void Controller::offsetScan (uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data)
{
    genericScan(test_offset, dac_start, dac_step, num_pulses, data);
}

void Controller::threshScan (uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data)
{
    genericScan(test_thresh, dac_start, dac_step, num_pulses, data);
}



















































// what the hell does this do
// double offset (double threshold) {
//     double secondary_amplitude = threshold * PULSEAMP_SCALE_FACTOR * ATTENUATION_MID;
//     double tertiary_amplitude  = threshold * PULSEAMP_SCALE_FACTOR * ATTENUATION_HIGH;
//
//     double offset=tertiary_amplitude-secondary_amplitude;
//     return offset;
// }

//std::string isPassed (bool pass)
//{
//    if (pass)
//        return "\x1b[32mPASS\x1b[0m";
//    else
//        return "\033[1m\x1b[31mFAIL\x1b[0m";
//}

//                 void timingScan()
//                 {
//                     initializeLCT();
//                     unsigned int errorSpots [16][120];
//                     pdac.write((1<<14)-1);
//
//
//                     while (!Comparator::isPulserReady());
//
//                     /* Configure Muxes and Write Pattern Expect */
//                     configurePulser (2, RIGHT);
//
//                     cdac.writeThreshold(10);
//                     Comparator::writeLCTReset (0);
//
//                     for (int bx_delay=0; bx_delay<16; bx_delay++) {
//                         Comparator::writeBxDelay(bx_delay);
//                         for (int ddd_delay=0; ddd_delay<120; ddd_delay++) {
//                             DDD::setDelay(ddd_delay);
//                             Comparator::resetHalfstripsErrcnt();
//
//                             for (int ipulse=0; ipulse < 100; ipulse++) {
//                                 Comparator::firePulse();
//                             }
//
//                             auto hs = Comparator::readHalfstrips();
//                             printf("%08X =?", hs);
//                             printf("%08X\n", Comparator::readPatternExpect().halfstrips);
//
//                             errorSpots [bx_delay][ddd_delay] = Comparator::readHalfstripsErrcnt();
//
//                             //if (errorSpots [bx_delay][ddd_delay]  < 9) {
//                             //    printf("%08X", Comparator::readHalfstrips());
//                             //}
//                             //printf("%i\n", errorSpots [bx_delay][ddd_delay]);
//                         }
//                     }
//
//                     char yaxis[17] = "    BX DELAY    ";
//                     char header[] = "     1       10        20        30        40        50        60        70        80        90       100       110       120\n";
//
//                     printf("                                                Pulse Injection (DDD) Delay \n");
//                     printf("%s", header);
//                     for (int bx_delay=0; bx_delay<16; bx_delay++) {
//                         printf("%c %02i ", yaxis[bx_delay], bx_delay);
//                         for (int ddd_delay=0; ddd_delay<120; ddd_delay++) {
//                             if (errorSpots[bx_delay][ddd_delay]>9)
//                                 printf("X");
//                             else
//                                 printf(" ");
//                         }
//                         printf("\n");
//                     }
//                 }


