#include <SPI.h>
#include "defaults.h"
#include "erflut.h"
#include "controller.h"
#include "global_objects.h"
#include "pinmodes.h"

bool debug = 0;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

char rx_msg [120];
uint16_t start = 0;

void loopBackStressTest () {

    SerialUSB.println("INFO::Starting Serial Loopback Test");
    for (int data = 0; data<0xffff; data=data+8) {
        fpga.writeAddress(0, uint16_t(data) );
        uint16_t readback = fpga.readAddress(uint8_t(0));
        if (readback!=data) {
            SerialUSB.print("LOOPBACK ERROR: expect=");
            SerialUSB.print(data);
            SerialUSB.print("  read=");
            SerialUSB.print(readback);
            SerialUSB.print("\r\n");
        }
//         else
//             SerialUSB.print("loopback ok:");
//             SerialUSB.print(data);
//             SerialUSB.print("\r\n");
//     }
    }
    SerialUSB.println("INFO::Loopback Test Finished");
}


void setup () {

    //------------------------------------------------------------------------------------------------------------------
    // Start Serial
    //------------------------------------------------------------------------------------------------------------------


    SerialUSB.begin(115200);

    setPinModes();

  //  while(!SerialUSB);


    //while (!SerialUSB.available()) {};

    //------------------------------------------------------------------------------------------------------------------
    // STARTUP
    //------------------------------------------------------------------------------------------------------------------

    SerialUSB.println("INFO::UCLA HEP LCT COMPARATOR TEST BOARD");

    SerialUSB.println("INFO::SOFTWARE VERSION 20170124");

    SerialUSB.println("INFO::STARTING SPI");
    SPI.begin();


    // fpga max is 4
    SPI.setClockDivider(SPI_CLOCK_DIV16); // DO NOT run faster than 4.. confirmed that it breaks at 2
  //SPI.setBitOrder(MSBFIRST);

    SerialUSB.println("INFO::STARTUP SEQUENCE FINISHED");

    controller.initialize();

    analogReadResolution(14);
    memset(rx_msg, 0, sizeof rx_msg);

    loopBackStressTest();

}

static const int size = 1024;

char msg [120];
uint16_t data_buffer [size];

bool done = 0;

enum {cmd_offset, cmd_thresh, cmd_current, cmd_wr, cmd_rd, cmd_pulse, cmd_scan, cmd_timescan, cmd_modescan};
uint32_t loop_cnt;
    uint8_t  rx_index = 0;

void loop () {

start:

    uint8_t   scan_channel;
    uint16_t  param1=1000;

    uint8_t  scan_side;  // side is really 8 bits but save the extras for doubling as register data
    uint16_t param2=10; // side is really 8 bits but save the extras for doubling as register data

    uint16_t param3=1;
    uint16_t param4=1;
    uint16_t param5;

    uint8_t  dac_step;

    uint16_t num_pulses;
    uint8_t  scan_cmd;


    if (SerialUSB.available()) {

        //--------------------------------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------------------------------

        // command     chan    side
        // thresh      0       0
        // offset      0       0


        // read in an entire line from serial (terminating on
        char  rx_char;

        uint16_t start_cnt = 0;


        while (SerialUSB.available() > 0) // Don't read unless
            // you know there is data
        {
            //if (debug) {
            //    if (start_cnt % 128 == 0)
            //        SerialUSB.println(start_cnt);
            //    start_cnt = start_cnt + 1;
            //}

            if (rx_index < 120) // One less than the size of the array
            {
                rx_char = SerialUSB.read(); // Read a character

                // if (debug) SerialUSB.print((char) rx_char);

                if (rx_char=='\n') {
                    rx_msg[rx_index-1] = '\0';             // Null terminate the string, replacing the \r read previously (assuming \r\n)
                    if (debug) SerialUSB.println("parsing string:");
                    if (debug) SerialUSB.println(rx_msg);
                    rx_index=0;
                    goto parse;
                }

                // if (debug) SerialUSB.println("rx index:");
                // if (debug) SerialUSB.println(rx_index);
                // if (debug) SerialUSB.println("rx_msg:");
                // if (debug) SerialUSB.println(rx_msg);
                // if (debug) SerialUSB.println("rx_char:");
                // if (debug) SerialUSB.println(rx_char);

                rx_msg[rx_index] = rx_char; // Store it
                rx_index = rx_index + 1;    // Increment where to write next
            }

            if (start_cnt > 10000)
                SerialUSB.println("info::timeout");
                goto start;
        }

parse:

        if (debug) SerialUSB.println("parsing:");
        if (debug) SerialUSB.println(rx_msg);

        char word_index     = 0;
        scan_cmd     = -1;
        scan_channel = 0;
        scan_side    = 0;
        param3       = 0;
        param4       = 1;
        param5       = 1;

        for (char *p = strtok(rx_msg," "); p != NULL; p = strtok(NULL, " ")) {
            // if (debug) SerialUSB.println("parsing:");
            // if (debug) SerialUSB.println(p);

            // SerialUSB.println("Parsing String:");
            // SerialUSB.println(p);
            // SerialUSB.println("Length of string:");
            // SerialUSB.println(unsigned(strlen(p)));

            //- parse command-------------------------------------------------------------------------------------------
            if (word_index==0) {
                if (strcmp (p,"offset")==0)
                    scan_cmd = cmd_offset;
                else if (strcmp (p, "thresh")==0)
                    scan_cmd = cmd_thresh;
                else if (strcmp (p, "current")==0)
                    scan_cmd = cmd_current;
                else if (strcmp (p, "wr")==0)
                    scan_cmd = cmd_wr;
                else if (strcmp (p, "rd")==0)
                    scan_cmd = cmd_rd;
                else if (strcmp (p, "pulse")==0)
                    scan_cmd = cmd_pulse;
                else if (strcmp (p, "scan")==0)
                    scan_cmd = cmd_scan;
                else if (strcmp (p, "timing")==0)
                    scan_cmd = cmd_timescan;
                else if (strcmp (p, "mode")==0)
                    scan_cmd = cmd_modescan;
                else if (strcmp (p, "reset")==0) {
                    sprintf(msg, "INFO::RESETTING CONTROLLER"); SerialUSB.println(msg);
                    setup();
                    goto start;
                }
            }

            // if (debug) SerialUSB.println("scancmd=");
            // if (debug) SerialUSB.println(scan_cmd);

            //- parse parameters----------------------------------------------------------------------------------------
            else if (word_index==1) {
                param1       = strtol(p,NULL,0);
                scan_channel = (uint8_t) param1;
                if (debug) SerialUSB.println("setting scan channel");
            }
            else if (word_index==2) {
                param2 = strtol(p,NULL,0);
                scan_side = (uint8_t) param2;
                if (debug) SerialUSB.println("setting scan side");
            }
            else if (word_index==3) {
                param3 = strtol(p,NULL,0);
                if (debug) SerialUSB.println("setting dac start");
            }
            else if (word_index==4) {
                param4 = strtol(p,NULL,0);
                dac_step = strtol(p,NULL,0);
            }
            else if (word_index==5) {
                param5 = strtol(p,NULL,0);
                num_pulses = strtol(p,NULL,0);
            }

            word_index++;
        }


        memset(rx_msg, 0, sizeof rx_msg);


        if (scan_cmd == cmd_offset) {
            scanOffset (uint8_t(param1), uint8_t(param2), param3, param4, param5, data_buffer);
        }

        else if (scan_cmd == cmd_thresh) {
            scanThresh (uint8_t(param1), uint8_t(param2), param3, param4, param5, data_buffer);
        }

        else if (scan_cmd == cmd_current) {
            scanCurrent (uint8_t(param1), data_buffer);
            //SerialUSB.print ("    ifamp=");
            //SerialUSB.print ((analogRead(0)*3.3)/16536);
            //SerialUSB.print ("    iamp=");
            //SerialUSB.print ((analogRead(1)*3.3)/16536);
            //SerialUSB.print ("    ioff=");
            //SerialUSB.print ((analogRead(2)*3.3)/16536);
            //SerialUSB.print ("    ibias=");
            //SerialUSB.print ((analogRead(3)*3.3)/16536);
            //SerialUSB.print ("    +5v0=");
            //SerialUSB.print ((analogRead(4)*3.3)/16536);
            //SerialUSB.print ("    +3v3=");
            //SerialUSB.print ((analogRead(5)*3.3)/16536);
            //SerialUSB.print ("\r\n");

        }
        else if (scan_cmd == cmd_wr) {
            writeRegister(uint8_t(param1), param2); // hijack these variables for our use
        }
        else if (scan_cmd == cmd_rd) {
            readRegister(uint8_t(param1)); // hijack these variables for our use
        }
        else if (scan_cmd == cmd_pulse) {
            controller.pulse(param1, param2, param3, param4, param5);
        }
        else if (scan_cmd == cmd_scan) {
            controller.scan(param1);
        }
        else if (scan_cmd == cmd_timescan) {
            sprintf(msg, "DATA::START=1 TEST=TIMING DAC_VALUE=%i NUM_PULSES=%i STRIP=%i SIDE=%i ", param1, param2, param3, param4);
            SerialUSB.println(msg);
            start = millis();
            controller.scanPeakTiming(param1, param2, param3, param4);
            transmitEndString();
        }
        else if (scan_cmd == cmd_modescan) {
            sprintf(msg, "DATA::START=1 TEST=MODE DAC_VALUE=%i NUM_PULSES=%i STRIP=%i SIDE=%i ", param1, param2, param3, param4);
            SerialUSB.println(msg);
            start = millis();
            controller.scanMode(param1, param2, param3, param4);
            transmitEndString();
        }
    }


}

void writeRegister(uint8_t address, uint16_t data) {
    sprintf(msg, "poke adr=0x%02X data=%04X", address, data);
    fpga.writeAddress(address, data);
    SerialUSB.println(msg);
}

void readRegister(uint8_t address) {
    uint16_t data = fpga.readAddress(address);
    sprintf(msg, "peek adr=0x%02X data=%04X", address, data);
    SerialUSB.println(msg);
}

void scanOffset (uint8_t strip, uint8_t side, uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data) {

    start = millis();

    sprintf(msg, "DATA::START=1 TEST=OFFSET  STRIP=%i SIDE=%i DAC_START=%i DAC_STEP=%i NUM_PULSES=%i", strip,side,dac_start,dac_step,num_pulses);
    SerialUSB.println(msg);

    transmitStartString();

    pulser.setStrip(strip, side, test_offset);

    controller.offsetScan(dac_start, dac_step, num_pulses, data);

    if (!split_packets) {
        transmitResultPacket(data);
    }

    SerialUSB.print ("\r\n");
    transmitEndString();
}

void scanThresh (uint8_t strip, uint8_t side, uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data) {

    start = millis();

    sprintf(msg, "DATA::START=1 TEST=THRESH  STRIP=%i SIDE=%i DAC_START=%i DAC_STEP=%i NUM_PULSES=%i", strip,side,dac_start,dac_step,num_pulses);
    SerialUSB.println(msg);

    transmitStartString();

    pulser.setStrip(strip, side, test_thresh);

    controller.threshScan(dac_start, dac_step, num_pulses, data);

    if (!split_packets) {
        transmitResultPacket(data);
    }

    SerialUSB.print ("\r\n");
    transmitEndString();
}

void scanCurrent (uint8_t channel, uint16_t* data) {

    start = millis();

    for (int i=0; i<(size); i++) {
        data[i] = analogRead(channel);
    }

    sprintf(msg, "DATA::START=1 TEST=CURRENT  CHANNEL=%i", channel);
    SerialUSB.println(msg);

    transmitStartString();
    transmitResultPacket(data);
    SerialUSB.print ("\r\n");
    transmitEndString();
}

void transmitResultPacket (uint16_t* data)
{
    // thresh 0 1 100 1 1000
    // 1 short per packet  = 20522 ms
    // 4 shorts per packet = 20513 ms
    // 8 shorts per packet = 20545 ms

    //    for (int ipacket=0; ipacket<64; ipacket+=1) {
    //        for (int icounter=0; icounter<size/256; icounter+=1) {
    //            int start = ipacket * 16 + icounter*4;
    //            sprintf(msg, "%04X%04X%04X%04X", (data[start]), (data[start+1]), (data[start+2]), (data[start+3]));
    //            SerialUSB.print(msg);
    //        }
    //
    //    }

    for (int ipacket=0; ipacket<1024; ipacket+=1) {
            sprintf (msg, "%04X", data[ipacket]);
            SerialUSB.print(msg);
    }

    //for (int ipacket=0; ipacket<32; ipacket+=1) {
    //    for (int icounter=0; icounter<8; icounter+=1) {
    //        int start = ipacket * 16 + icounter*8;
    //        sprintf (msg, "%04X%04X%04X%04X%04X%04X%04X%04X", data[start], data[start+1], data[start+2], data[start+3], data[start+4], data[start+5], data[start+6], data[start+7] );
    //        SerialUSB.print(msg);
    //    }

    //}

}

void transmitStartString ()
{
    SerialUSB.print ("BIN:::");
}

void transmitEndString ()
{
    uint16_t time = millis() - start;
    uint16_t packet_size = (size * 2) / 1000;

    sprintf(msg, "DATA::END=1 RUNTIME=%u TXSIZE=%u", time, packet_size); SerialUSB.println(msg);
}
