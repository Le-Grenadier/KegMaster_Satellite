
#include <stdio.h>

#include "tsk_timer.h"

uint24_t TSK_timer;

// TODO: Use carry flag - make system timers better. 

/* Task timer overflows every 4.6 hours */
uint24_t TSK_timer_get(){
    return(TSK_timer);
}

