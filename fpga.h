#ifndef FPGA_H
#define FPGA_H

#include "register.h"
#include "fpga_fields.h"
#include "spi.h"
#include <stdint.h>
#include <Arduino.h>
#include "pinout.h"

class Fpga {

    public:

        void writeAddress (Reg* reg) {

            digitalWrite(fpga_cs_pin, 0);
            digitalWrite(fpga_wren_pin, 0);

            spin::transfer   (reg->getAdr());
            spin::transfer16 (reg->getData());

            digitalWrite(fpga_cs_pin, 1);
            digitalWrite(fpga_wren_pin, 1);
        }

        void writeAddress (uint8_t address) {

            writeAddress (reg_array[address]);
        }

        void readAddress (Reg* reg) {

            digitalWrite(fpga_cs_pin, 0);
            digitalWrite(fpga_wren_pin, 1); // wren = 1 for read-only

            spin::transfer   (reg->getAdr());

            reg->set(spin::transfer16 (0) );

            digitalWrite(fpga_cs_pin, 1);
            digitalWrite(fpga_wren_pin, 1);
        }

        void readAddress (uint8_t address) {

            Reg* reg = reg_array[address];

            readAddress (reg);
        }

        void readAll () {
            for ( int i = 0; i<adr_last; i++ )
                readAddress (reg_array[i]);
        }

        void writeAll () {
            for ( int i = 0; i<adr_last; i++ )
                writeAddress (reg_array[i]);
        }

        void fire()
        {
            digitalWrite(fpga_fire_pin, 1);
            digitalWrite(fpga_fire_pin, 0);
        }

        // go from a per strip amplitude configuration; guess a expected halfstrip response.

        bool isReady ()
        {
            return digitalRead(fpga_ready_pin);
        }

        void resetCounters ()
        {
            digitalWrite(fpga_reset_pin, 1);
            digitalWrite(fpga_reset_pin, 0);
        }
};

#endif /* FPGA_H */
