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

    ddd.setDelay(DDD_DELAY);

    cdac.write(0);
    pdac.write(0);

    //------------------------------------------------------------------------------------------------------------------
    // Default FPGA Configuration
    //------------------------------------------------------------------------------------------------------------------

    fpga.readAll();

    bx_delay.set       (BX_DELAY);
    pulse_width.set    (PULSE_WIDTH);
    pkmode.set         (PKMODE);
    pktime.set         (PKTIME);
    triad_persist.set  (TRIAD_PERSIST);
    triad_persist1.set (TRIAD_PERSIST1);
    compin_inject.set  (COMPIN_INJECT);
    lctrst.set         (1);

    fpga.writeAll();
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
        cdac.writeThreshold(0); //
    }

    delayMicroseconds(100);

    /* wait for pulser ready... poll the board until it is OK. */
    // while (fpga.isReady()) ;

    // don't ask where this magic number came from...
    // its the number of different settings in a for loop that starts at 1 and skips values
    // it was mostly by trial and error but seems to be reliable (tested by python script)

    //static const int NUM_SETTINGS = max/step + 1 - (max % step <=1) - (max % step==0 && step==1);

    fire_num_pulses.set(num_pulses);
    fpga.writeAddress(fire_num_pulses.adr());

    for (int index=0; index<1024; index++) {

        uint16_t dac_value = dac_start + index*dac_step;

        /* wait some delay for DAC to settle initially .. subsequent incremental changes can be faster */
        pdac.write(dac_value);

        if (dac_value==dac_start)
            delayMicroseconds(100);
        else
            delayMicroseconds(10);

        fpga.resetCounters();

        //for (int ipulse=0; ipulse < 10; ipulse++) {
        //    digitalWrite(6, 1);
        //    digitalWrite(6, 0);
        fpga.fire();
//            while (fpga.isReady()) ;
//        }

        bitField *field = (test_type==test_thresh) ? &thresholds_errcnt : &offsets_errcnt;

        fpga.readAddress (field->getAdr());

        // optionally just send an error function from RAM
        static const int useerf = 1;

        if (useerf)
            data[index] = erflut[index];
        else
            data[index] = field->get();
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


