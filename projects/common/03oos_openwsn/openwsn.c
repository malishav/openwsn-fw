/**
   \brief This project runs the full OpenWSN stack.

   \author Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2010
 */

#include "board.h"
#include "scheduler.h"
#include "stack.h"

int mote_main(void) {

    // initialize
    board_init();
    scheduler_init();
    stack_init();

    // start
    scheduler_start();

    return 0; // this line should never be reached
}
