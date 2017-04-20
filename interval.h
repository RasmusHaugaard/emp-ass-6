#ifndef INTERVAL_H_
#define INTERVAL_H_

#include <stdint.h>
#include "emp_type.h"
#include "glob_def.h"

#define MAX_INTERVALS 16

typedef struct {
    INT16U tpi;
    INT16U last_ticks;
    BOOLEAN overflowed;
} interval;

INT8U create_interval(INT16U tpi); // use millis() or micros() to get tickcount
BOOLEAN check_interval(INT8U);

#endif
