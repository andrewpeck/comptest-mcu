#include <SPI.h>
#include "erflut.h"
#include "controller.h"
#include "global_objects.h"

void(* resetFunc) (void) = 0; //declare reset function @ address 0

uint16_t start = 0;
void setup () {

    //------------------------------------------------------------------------------------------------------------------
    // Start Serial
    //------------------------------------------------------------------------------------------------------------------

    SerialUSB.begin(115200);


// while(!SerialUSB);


    while (!SerialUSB.available()) {};

    //------------------------------------------------------------------------------------------------------------------
    // STARTUP
    //------------------------------------------------------------------------------------------------------------------

    SerialUSB.println("INFO::UCLA HEP LCT COMPARATOR TEST BOARD");

    SerialUSB.println("INFO::SOFTWARE VERSION 20170124");

    SerialUSB.println("INFO::STARTING SPI");
    SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV8);
  //SPI.setBitOrder(MSBFIRST);

    SerialUSB.println("INFO::STARTUP SEQUENCE FINISHED");

    // analogReadResolution(16);

}

static const int size = 1024;

char msg [120];
uint16_t data_buffer [size];
char rx_msg [120];
Controller controller;

bool done = 0;

enum {cmd_offset, cmd_thresh, cmd_current};

void loop () {

start:

    uint8_t scan_channel;
    uint8_t scan_side;
    uint16_t dac_start;
    uint8_t dac_step;
    uint16_t num_pulses;
    uint8_t scan_cmd;

    if (SerialUSB.available()) {

        //--------------------------------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------------------------------

        // command     chan    side
        // thresh      0       0
        // offset      0       0


        // read in an entire line from zerial (terminating on
        uint8_t  rx_index = 0;
        char  rx_char;

        uint16_t start_cnt = 0;

        memset(rx_msg, 0, sizeof rx_msg);


        while (SerialUSB.available() > 0) // Don't read unless
            // there you know there is data
        {
            //if (start_cnt % 128 == 0)
            //    SerialUSB.println(start_cnt);
            start_cnt = start_cnt + 1;

            if(rx_index < 120) // One less than the size of the array
            {
                rx_char          = SerialUSB.read(); // Read a character
                // SerialUSB.print((char) rx_char);

                if (rx_char=='\n') {
                    rx_msg[rx_index-1] = '\0';             // Null terminate the string, replacing the \r read previously (assuming \r\n)
                    goto parse;
                }

                rx_msg[rx_index] = rx_char;          // Store it
                rx_index         = rx_index + 1;     // Increment where to write next


            }

            if (start_cnt > 10000)
                SerialUSB.println("info::timeout);
                goto start;
        }

parse:
        rx_index     = 0;
        scan_cmd     = -1;
        scan_channel = 0;
        scan_side    = 0;
        dac_start    = 0;
        dac_step     = 1;
        num_pulses   = 1;

        for (char *p = strtok(rx_msg," "); p != NULL; p = strtok(NULL, " "))
        {

            // SerialUSB.println("Parsing String:");
            // SerialUSB.println(p);
            // SerialUSB.println("Length of string:");
            // SerialUSB.println(unsigned(strlen(p)));

            //- parse command-------------------------------------------------------------------------------------------
            if (rx_index==0) {
                if (strcmp (p,"offset")==0)
                    scan_cmd = cmd_offset;
                else if (strcmp (p, "thresh")==0)
                    scan_cmd = cmd_thresh;
                else if (strcmp (p, "current")==0)
                    scan_cmd = cmd_current;
                else if (strcmp (p, "reset")==0) {
                    sprintf(msg, "INFO::RESETTING CONTROLLER"); SerialUSB.println(msg);
                    setup();
                    goto start;
                }
            }

            //- parse parameters----------------------------------------------------------------------------------------
            else if (rx_index==1) {
                scan_channel = strtol(p,NULL,0);
            }
            else if (rx_index==2) {
                scan_side = strtol(p,NULL,0);
            }
            else if (rx_index==3) {
                dac_start = strtol(p,NULL,0);
            }
            else if (rx_index==4) {
                dac_step = strtol(p,NULL,0);
            }
            else if (rx_index==5) {
                num_pulses = strtol(p,NULL,0);
            }

            rx_index++;
        }

        if (scan_cmd == cmd_offset) {
            scanOffset (scan_channel, scan_side, dac_start, dac_step, num_pulses, data_buffer);
        }

        else if (scan_cmd == cmd_thresh) {
            scanThresh (scan_channel, scan_side, dac_start, dac_step, num_pulses, data_buffer);
        }

        else if (scan_cmd == cmd_current) {
            scanCurrent (scan_channel, data_buffer);
        }
    }
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
