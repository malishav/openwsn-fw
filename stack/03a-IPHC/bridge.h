#ifndef OPENWSN_OPENBRIDGE_H
#define OPENWSN_OPENBRIDGE_H

/**
\addtogroup LoWPAN
\{
\addtogroup OpenBridge
\{
*/

#include "queue.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================

void openbridge_init(void);

void openbridge_triggerData(void);

void openbridge_sendDone(QueueEntry_t *msg, owerror_t error);

void openbridge_receive(QueueEntry_t *msg);

/**
\}
\}
*/

#endif /* OPENWSN_OPENBRIDGE_H */
