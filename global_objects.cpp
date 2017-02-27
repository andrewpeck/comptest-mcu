#include "pinout.h"
#include "global_objects.h"

CDAC cdac (cdac_en_pin, 3.3);
PDAC pdac (pdac_en_pin, 3.3);
DDD  ddd (0, 0, ddd_al_pin); // need to actually assign this
Fpga fpga;
Pulser pulser;
