#ifndef GLOBAL_OBJECTS_H
#define GLOBAL_OBJECTS_H

#include "pulser.h"
#include "dac.h"
#include "ddd.h"
#include "fpga.h"


extern Fpga fpga;
extern Pulser pulser;
extern CDAC cdac;
extern PDAC pdac;
extern DDD  ddd;

#endif /* GLOBAL_OBJECTS_H */
