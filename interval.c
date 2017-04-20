#include "interval.h"
#include "rtcs.h"

interval poi[MAX_INTERVALS];

INT8U create_interval(INT16U tpi){
    static INT8U next_id = 0;
    INT8U id = next_id++;
    poi[id].last_ticks = now_ticks();
    poi[id].overflowed = FALSE;
    poi[id].tpi = tpi;
    return id;
}

BOOLEAN check_interval(INT8U id){
    INT16U delta_ticks = now_ticks() - poi[id].last_ticks;
    BOOLEAN result = FALSE;
    if (delta_ticks >= poi[id].tpi){
        if (delta_ticks >= poi[id].tpi * 2)
            poi[id].overflowed = TRUE;
        poi[id].last_ticks += poi[id].tpi;
        result = TRUE;
    }
    return result;
}
