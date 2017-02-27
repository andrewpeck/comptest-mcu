#ifndef PINMODES_H
#define PINMODES_H 

inline void setPinModes () {

    // configure pin directions
    pinMode (v_ifamp_pin        , INPUT);
    pinMode (v_iamp_pin         , INPUT);
    pinMode (v_ioff_pin         , INPUT);
    pinMode (v_ibias_pin        , INPUT);
    pinMode (comp_sense_5v5_pin , INPUT);
    pinMode (comp_sense_3v3_pin , INPUT);
    pinMode (ddd_al_pin         , OUTPUT);
    pinMode (pdac_en_pin        , OUTPUT);
    pinMode (cdac_en_pin        , OUTPUT);
    pinMode (samd_io0           , OUTPUT); // fire
    pinMode (samd_io1           , INPUT); // ready
    pinMode (samd_io2           , OUTPUT); // reset
    pinMode (samd_io3           , INPUT);
    pinMode (samd_io4           , INPUT);
    pinMode (samd_io5           , INPUT);

    // set default state for outputs
    digitalWrite (ddd_al_pin, HIGH);
    digitalWrite (pdac_en_pin, HIGH);
    digitalWrite (cdac_en_pin, HIGH);
}

#endif /* PINMODES_H */
