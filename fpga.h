#ifndef FPGA_H
#define FPGA_H

#include "register.h"
#include "fpga_fields.h"
#include "spi.h"
#include <stdint.h>
#include <Arduino.h>
#include "pinmodes.h"

class Fpga {

    public:

        void writeAddress (uint8_t address, uint16_t data) {

//
//           SerialUSB.print("writing adr=");
//           SerialUSB.print(address);
//           SerialUSB.print("  data=");
//           SerialUSB.print(data);
//           SerialUSB.print("\r\n");
//
            setFpgaWrEn(1);
            setFpgaCs(1);

            spin::transfer   (address);
            spin::transfer16 (data);

            setFpgaWrEn(0);
            setFpgaCs(0);

            address = 0;
            readAddress(address);
        }

        void writeAddress (Reg* reg) {
            uint8_t adr = reg->getAdr();
            uint16_t data = reg->getData();

           // SerialUSB.print("intent writing adr=");
           // SerialUSB.print(adr);
           // SerialUSB.print("  data=");
           // SerialUSB.print(data);
           // SerialUSB.print("\r\n");


            writeAddress(adr, data);
        }

        void writeAddress (uint8_t address) {
            writeAddress (reg_array[address]);
        }

        void readAddress (Reg* reg) {

            uint8_t address = reg->getAdr();
            uint16_t data = readAddress(address);
            reg->set(data);
        }

        uint16_t readAddress (uint8_t address) {


//            SerialUSB.print("reading adr=");
//            SerialUSB.print(address);

            setFpgaCs(1);
            spin::transfer   (address);
            uint16_t read_data = (spin::transfer16 (0) );
            setFpgaCs(0);

//            SerialUSB.print("  data=");
//            SerialUSB.print(read_data, HEX);
//            SerialUSB.print("\r\n");
//

            return read_data;
        }

        void readAll () {
            for ( int i = 0; i<adr_last; i++ )
                readAddress (reg_array[i]);
        }

        void writeAll () {
            for ( int i = 1; i<adr_last; i++ ) // don't overwrite the loopback
                writeAddress (reg_array[i]);
        }

        void fire()
        {
            setFpgaFire(1);
            delayMicroseconds(10);
            setFpgaFire(0);
        }

        // go from a per strip amplitude configuration; guess a expected halfstrip response.

        bool isReady ()
        {
            //return digitalRead(pa13);
            bool state =  (REG_PORT_IN0 & fpga_ready_pin)!=0;
            //SerialUSB.print("state=");
            //SerialUSB.print(state);
            //SerialUSB.print("\r\n");
            return (state);
        }

        void resetCounters ()
        {
            setFpgaReset(1);
            delayMicroseconds(1);
            setFpgaReset(0);
        }
};

#endif /* FPGA_H */
