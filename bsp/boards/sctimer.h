#ifndef __SCTIMER_H
#define __SCTIMER_H

/**
\addtogroup BSP
\{
\addtogroup sctimer
\{

\brief A timer module with only a single compare value.

\author Tengfei Chang <tengfei.chang@inria.fr>, April 2017.
*/

#include "stdint.h"
#include "board.h"

//=========================== typedef =========================================

typedef void  (*sctimer_cbt)(void);

//=========================== variables =======================================


//=========================== prototypes ======================================

void     sctimer_init(void);
void     sctimer_setCompare(PORT_TIMER_WIDTH val);
void     sctimer_set_callback(sctimer_cbt cb);
PORT_TIMER_WIDTH sctimer_readCounter(void);
void     sctimer_enable(void);
void     sctimer_disable(void);

/**
\}
\}
*/

#endif
