#ifndef FIELDS_H
#define FIELDS_H

#include "register.h"

enum {
    adr_loopback          = 0  ,
    adr_comp_config       = 1  ,
    adr_fire_pulse        = 2  ,
    adr_mux_ctrl          = 3  ,
    adr_pulse_ctrl        = 4  ,
    adr_pulse_ctrl2       = 5  ,
    adr_halfstrips        = 6  ,
    adr_halfstrips2       = 7  ,
    dummy0                = 8  ,
    adr_active_strip_mask = 9  ,
    dummy1                = 10 ,
    adr_offsets_errcnt    = 11 ,
    adr_compout_errcnt    = 12 ,
    adr_thresholds_errcnt = 13 ,
    adr_restore_cnt       = 14 ,
    adr_response_time     = 15 ,
    adr_last              = 16
};

extern Reg reg_loopback;
extern bitField loopback;

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


extern Reg reg_active_strip_mask;
extern bitField active_halfstrip;
extern bitField halfstrip_mask_en;

extern Reg reg_offsets_errcnt ;
extern bitField offsets_errcnt ;

extern Reg reg_compout_errcnt ;
extern bitField compout_errcnt ;

extern Reg reg_thresholds_errcnt ;
extern bitField thresholds_errcnt ;

extern Reg reg_restore_cnt ;
extern bitField restore_cnt ;

extern Reg reg_response_time ;
extern bitField response_time ;

// hacky way of indexing registers by their addresses

extern Reg dum0;
extern Reg dum1;

extern Reg* reg_array [adr_last];

#endif /* FIELDS_H */
