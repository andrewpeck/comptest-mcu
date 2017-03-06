#include "pinout.h"
#include "global_objects.h"

CDAC cdac;
PDAC pdac;
DDD  ddd (0, 0, ddd_al_pin); // need to actually assign this
Fpga fpga;
Pulser pulser;
Controller controller;
