#include "fpga_fields.h"

Reg reg_loopback(adr_loopback);
bitField loopback (&reg_loopback, 0, 15);

Reg reg_comp_config(adr_comp_config);
bitField pktime (&reg_comp_config, 0, 2);
bitField pkmode (&reg_comp_config, 3, 4);
bitField lctrst (&reg_comp_config, 5, 5);

Reg                              reg_fire_pulse   (adr_fire_pulse);
bitField fire_pulse            (&reg_fire_pulse, 0, 0);
bitField offset_errcnt_rst     (&reg_fire_pulse, 1, 1);
bitField compout_errcnt_rst    (&reg_fire_pulse, 2, 2);
bitField thresholds_errcnt_rst (&reg_fire_pulse, 3, 3);
bitField fire_num_pulses       (&reg_fire_pulse, 4, 15);

Reg reg_pulse_ctrl (adr_pulse_ctrl);
bitField pulse_width (&reg_pulse_ctrl, 1, 4);
bitField compin_inject (&reg_pulse_ctrl, 10, 10);
bitField bx_delay (&reg_pulse_ctrl, 11, 14);
bitField compout_expect (&reg_pulse_ctrl, 15, 15);

Reg reg_pulse_ctrl2 (adr_pulse_ctrl2);
bitField triad_persist (&reg_pulse_ctrl2, 0, 3);
bitField triad_persist1 (&reg_pulse_ctrl2, 4, 4);
bitField mux_en (&reg_pulse_ctrl2, 5, 5);
bitField compout_last (&reg_pulse_ctrl2, 6, 6);
bitField pulser_ready (&reg_pulse_ctrl2, 7, 7);

Reg reg_mux_ctrl (adr_mux_ctrl);

bitField mux_prev (&reg_mux_ctrl, 0,1);

bitField mux_next (&reg_mux_ctrl, 2,3);

bitField mux_high(&reg_mux_ctrl, 4,7);
bitField mux_med(&reg_mux_ctrl, 8,11);
bitField mux_low(&reg_mux_ctrl, 12,15);


Reg reg_halfstrips (adr_halfstrips);
Reg reg_halfstrips2 (adr_halfstrips2);
bitField halfstrips_lsbs (&reg_halfstrips, 0, 15);
bitField halfstrips_msbs (&reg_halfstrips2, 0, 15);

Reg reg_active_strip_mask (adr_active_strip_mask);
bitField active_halfstrip (&reg_active_strip_mask, 0, 4);

bitField halfstrip_mask_en (&reg_active_strip_mask, 5, 5);

Reg reg_offsets_errcnt (adr_offsets_errcnt);
bitField offsets_errcnt (&reg_offsets_errcnt, 0, 15);

Reg reg_compout_errcnt (adr_compout_errcnt);
bitField compout_errcnt (&reg_compout_errcnt, 0, 15);

Reg reg_thresholds_errcnt (adr_thresholds_errcnt);
bitField thresholds_errcnt (&reg_thresholds_errcnt, 0, 15);

Reg dum0 (dummy0);
Reg dum1 (dummy0);

// hacky way of indexing registers by their addresses
Reg* reg_array [adr_last] = {
    &reg_loopback          ,
    &reg_comp_config       ,
    &reg_fire_pulse        ,
    &reg_mux_ctrl          ,
    &reg_pulse_ctrl        ,
    &reg_pulse_ctrl2       ,
    &reg_halfstrips        ,
    &reg_halfstrips2       ,
    &dum0                ,
    &reg_active_strip_mask ,
    &dum1                ,
    &reg_offsets_errcnt    ,
    &reg_compout_errcnt    ,
    &reg_thresholds_errcnt ,
};
