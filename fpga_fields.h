#ifndef FIELDS_H
#define FIELDS_H

#include "register.h"

enum {
    adr_loopback          ,
    adr_comp_config       ,
    adr_fire_pulse        ,
    adr_mux_ctrl          ,
    adr_pulse_ctrl        ,
    adr_pulse_ctrl2       ,
    adr_halfstrips        ,
    adr_halfstrips2       ,
    adr_halfstrips_expect ,
    adr_halfstrips_expect2,
    adr_active_strip_mask ,
    adr_offsets_errcnt    ,
    adr_compout_errcnt    ,
    adr_thresholds_errcnt ,
    adr_last
};

extern Reg reg_loopback;
extern bitField loopback ;

extern Reg reg_comp_config;
extern bitField pktime ;
extern bitField pkmode ;
extern bitField lctrst ;

extern Reg reg_fire_pulse ;
extern bitField fire_pulse ;
extern bitField offset_errcnt_rst ;
extern bitField compout_errcnt_rst ;
extern bitField thresholds_errcnt_rst ;
extern bitField fire_num_pulses;

extern Reg reg_pulse_ctrl ;
extern bitField pulse_width ;
extern bitField compin_inject ;
extern bitField bx_delay ;
extern bitField compout_expect ;

extern Reg reg_pulse_ctrl2 ;
extern bitField triad_persist ;
extern bitField triad_persist1 ;
extern bitField mux_en ;
extern bitField compout_last ;
extern bitField pulser_ready ;

extern Reg reg_mux_ctrl ;

extern bitField mux_prev ;

extern bitField mux_next ;

extern bitField mux_high;
extern bitField mux_med;
extern bitField mux_low;


extern Reg reg_halfstrips ;
extern Reg reg_halfstrips2 ;
extern bitField halfstrips_lsbs ;
extern bitField halfstrips_msbs ;

extern Reg reg_halfstrips_expect ;
extern Reg reg_halfstrips_expect2;
extern bitField halfstrips_expect_lsbs ;
extern bitField halfstrips_expect_msbs ;


extern Reg reg_active_strip_mask ;
extern bitField active_strip_mask ;

extern Reg reg_offsets_errcnt ;
extern bitField offsets_errcnt ;

extern Reg reg_compout_errcnt ;
extern bitField compout_errcnt ;

extern Reg reg_thresholds_errcnt ;
extern bitField thresholds_errcnt ;

// hacky way of indexing registers by their addresses

extern Reg* reg_array [adr_last];

#endif /* FIELDS_H */
