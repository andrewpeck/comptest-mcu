#include <SPI.h>
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

    // analogReadResolution(16);
    memset(rx_msg, 0, sizeof rx_msg);

    loopBackStressTest();

}

static const int size = 1024;

char msg [120];
uint16_t data_buffer [size];

bool done = 0;

enum {cmd_offset, cmd_thresh, cmd_current, cmd_wr, cmd_rd};
uint32_t loop_cnt;
    uint8_t  rx_index = 0;

void loop () {

//    while (true) {
//        for (uint16_t dacs=0; dacs<65536; dacs++) {
//            pdac.write(dacs);
//            cdac.write(dacs);
//            if (dacs==0)
//                delayMicroseconds (1000);
//        }
//    }

//     bool state = 0;
//     while (true) {
//             // put your main code here, to run repeatedly:
// //            REG_PORT_OUTSET1 = PORT_PB11;     // Switch the output to 1 or HIGH
// //            REG_PORT_OUTSET1 = PORT_PB10;     // Switch the output to 1 or HIGH
// //            REG_PORT_OUTSET0 = PORT_PA08;     // Switch the output to 1 or HIGH
//         setFpgaWrEn(0);
//         setFpgaCs(0);
//             delayMicroseconds(10);
//         setFpgaWrEn(1);
//         setFpgaCs(1);
// //            REG_PORT_OUTCLR1 = PORT_PB11;     // Switch the output to 0 or LOW
// //            REG_PORT_OUTCLR1 = PORT_PB10;     // Switch the output to 0 or LOW
// //            REG_PORT_OUTCLR0 = PORT_PA08;      // Switch the output to 0 or LOW
//             delayMicroseconds(10);
//     }

//            while (true) {
//            digitalWrite(fpga_cs_pin,   0);
//            digitalWrite(fpga_wren_pin, 0);
//            digitalWrite(samd_io0, 0);
//            digitalWrite(samd_io1, 0);
//            digitalWrite(samd_io2, 0);
//            digitalWrite(samd_io3, 0);
//            digitalWrite(samd_io4, 0);
//            digitalWrite(samd_io5, 0);
//
////            spin::transfer   (address);
////            spin::transfer16 (data);
//            delayMicroseconds(500);
//
//            digitalWrite(fpga_cs_pin,   1);
//            digitalWrite(fpga_wren_pin, 1);
//            digitalWrite(samd_io0, 1);
//            digitalWrite(samd_io1, 1);
//            digitalWrite(samd_io2, 1);
//            digitalWrite(samd_io3, 1);
//            digitalWrite(samd_io4, 1);
//            digitalWrite(samd_io5, 1);
//            delayMicroseconds(500);
//            }

    //if (debug) {
//        loop_cnt++;
//        if (loop_cnt % 500 == 0) {
//            digitalWrite(fpga_cs_pin,   !digitalRead(fpga_cs_pin));
//            digitalWrite(fpga_wren_pin, !digitalRead(fpga_wren_pin));
//            SerialUSB.println("heartbeat...");
//        }
    //}

start:

    uint8_t scan_channel;
    uint8_t scan_side; // side is really 8 bits but save the extras for doubling as register data
    uint16_t write_data; // side is really 8 bits but save the extras for doubling as register data
    uint16_t dac_start;
    uint8_t dac_step;
    uint16_t num_pulses;
    uint8_t scan_cmd;


    if (SerialUSB.available()) {
        //SerialUSB.println("dav");

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
        dac_start    = 0;
        dac_step     = 1;
        num_pulses   = 1;

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
                scan_channel = strtol(p,NULL,0);
             if (debug) SerialUSB.println("setting scan channel");
            }
            else if (word_index==2) {
                write_data = strtol(p,NULL,0);
                scan_side = (uint8_t) write_data;
             if (debug) SerialUSB.println("setting scan side");
            }
            else if (word_index==3) {
                dac_start = strtol(p,NULL,0);
             if (debug) SerialUSB.println("setting dac start");
            }
            else if (word_index==4) {
                dac_step = strtol(p,NULL,0);
            }
            else if (word_index==5) {
                num_pulses = strtol(p,NULL,0);
            }

            word_index++;
        }


        memset(rx_msg, 0, sizeof rx_msg);


        if (scan_cmd == cmd_offset) {
            scanOffset (scan_channel, scan_side, dac_start, dac_step, num_pulses, data_buffer);
        }

        else if (scan_cmd == cmd_thresh) {
            scanThresh (scan_channel, scan_side, dac_start, dac_step, num_pulses, data_buffer);
        }

        else if (scan_cmd == cmd_current) {
            scanCurrent (scan_channel, data_buffer);
        }
        else if (scan_cmd == cmd_wr) {
            writeRegister(scan_channel, write_data); // hijack these variables for our use
        }
        else if (scan_cmd == cmd_rd) {
            readRegister(scan_channel); // hijack these variables for our use
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

    pulser.setStrip(strip, side);

    controller.offsetScan(dac_start, dac_step, num_pulses, data);

    transmitResultPacket(data);
}

void scanThresh (uint8_t strip, uint8_t side, uint16_t dac_start, uint16_t dac_step, uint16_t num_pulses, uint16_t* data) {

    start = millis();

    sprintf(msg, "DATA::START=1 TEST=THRESH  STRIP=%i SIDE=%i DAC_START=%i DAC_STEP=%i NUM_PULSES=%i", strip,side,dac_start,dac_step,num_pulses);
    SerialUSB.println(msg);

    pulser.setStrip(strip, side);

    controller.threshScan(dac_start, dac_step, num_pulses, data);

    transmitResultPacket(data);
}

void scanCurrent (uint8_t channel, uint16_t* data) {

    start = millis();

    for (int i=0; i<(size); i++) {
        data[i] = analogRead(channel);
    }

    sprintf(msg, "DATA::START=1 TEST=CURRENT  CHANNEL=%i", channel);
    SerialUSB.println(msg);
    transmitResultPacket(data);
}

void transmitResultPacket (uint16_t* data)
{

    SerialUSB.print ("BIN:::");

    for (int ipacket=0; ipacket<64; ipacket+=1) {
        for (int icounter=0; icounter<size/256; icounter+=1) {
            int start = ipacket * 16 + icounter*4;
            sprintf(msg, "%04X%04X%04X%04X", (data[start]), (data[start+1]), (data[start+2]), (data[start+3]));
            SerialUSB.print(msg);
        }

    }

    SerialUSB.print ("\r\n");

    uint16_t time = millis() - start;
    uint16_t packet_size = (size * 2) / 1000;

    sprintf(msg, "DATA::END=1 RUNTIME=%u TXSIZE=%u", time, packet_size); SerialUSB.println(msg);
}



