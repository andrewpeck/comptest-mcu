#ifndef PINMODES_H
#define PINMODES_H
#include "sam.h"
#include "pinout.h"

inline void writeDirectPort0 (uint32_t pin, bool state) {
    if (state) REG_PORT_OUTSET0 = pin;
    else       REG_PORT_OUTCLR0 = pin;
}

inline void writeDirectPort1 (uint32_t pin, bool state) {
    if (state) REG_PORT_OUTSET1 = pin;
    else       REG_PORT_OUTCLR1 = pin;
}

inline void setCdacEn (bool state) {
    if (state) REG_PORT_OUTSET0 = cdac_en_pin;
    else       REG_PORT_OUTCLR0 = cdac_en_pin;
}

inline void setPdacEn (bool state) {
    if (state) REG_PORT_OUTSET0 = pdac_en_pin;
    else       REG_PORT_OUTCLR0 = pdac_en_pin;
}

inline void setFpgaFire (bool state) {
    if (state) REG_PORT_OUTSET0 = fpga_fire_pin;
    else       REG_PORT_OUTCLR0 = fpga_fire_pin;
}

inline void setDddAl (bool state) {
    if (state) REG_PORT_OUTSET1 = ddd_al_pin;
    else       REG_PORT_OUTCLR1 = ddd_al_pin;
}

inline void setFpgaCs (bool state) {
    if (state) REG_PORT_OUTSET1 = fpga_cs_pin;
    else       REG_PORT_OUTCLR1 = fpga_cs_pin;
}

inline void setFpgaWrEn (bool state) {
    if (state) REG_PORT_OUTSET1 = fpga_wren_pin;
    else       REG_PORT_OUTCLR1 = fpga_wren_pin;
}

inline void setFpgaReset (bool state) {
    if (state) REG_PORT_OUTSET0 = fpga_reset_pin;
    else       REG_PORT_OUTCLR0 = fpga_reset_pin;
}

inline void setPinModes () {

    // configure pin directions
    pinMode (v_ifamp_pin        , INPUT);
    pinMode (v_iamp_pin         , INPUT);
    pinMode (v_ioff_pin         , INPUT);
    pinMode (v_ibias_pin        , INPUT);
    pinMode (comp_sense_5v5_pin , INPUT);
    pinMode (comp_sense_3v3_pin , INPUT);



    ////  INPUTS ?????????????????

    //pinMode (pa13     , INPUT);  // ready
    // pinMode (samd_io5     , INPUT); // unused

    // port b pins, set direction
    REG_PORT_DIRSET1 = fpga_cs_pin;   // Set the direction of the port pin to an output
    REG_PORT_DIRSET1 = fpga_wren_pin;
    REG_PORT_DIRSET1 = ddd_al_pin;

    // port a pins, set direction
    REG_PORT_DIRSET0 = fpga_wren_pin; // Set the direction of the port pin to an output
    REG_PORT_DIRSET0 = pdac_en_pin;
    REG_PORT_DIRSET0 = cdac_en_pin;
    REG_PORT_DIRSET0 = fpga_fire_pin;
    REG_PORT_DIRSET0 = fpga_reset_pin;


    //------------------------------------------------------------------------------------------------------------------
    // port a pins, set default state
    //------------------------------------------------------------------------------------------------------------------

    setPdacEn(1);
    setCdacEn(1);
    setFpgaFire(0);
    setFpgaReset(0);

    //------------------------------------------------------------------------------------------------------------------
    // port b pins, set default state
    //------------------------------------------------------------------------------------------------------------------

    // port b pins, set direction
    setFpgaCs(0);
    setFpgaWrEn(0);
    setDddAl(1);
}

#endif /* PINMODES_H */
