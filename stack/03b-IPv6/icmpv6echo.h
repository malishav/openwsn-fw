#ifndef OPENWSN_ICMPv6ECHO_H
#define OPENWSN_ICMPv6ECHO_H

/**
\addtogroup IPv6
\{
\addtogroup ICMPv6Echo
\{
*/

#include "queue.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== module variables ================================

typedef struct {
    bool busySending;
    open_addr_t hisAddress;
    uint16_t seq;
} icmpv6echo_vars_t;

//=========================== prototypes ======================================

void icmpv6echo_init(void);

void icmpv6echo_trigger(void);

void icmpv6echo_sendDone(QueueEntry_t *msg, owerror_t error);

void icmpv6echo_receive(QueueEntry_t *msg);

/**
\}
\}
*/

#endif /* OPENWSN_ICMPV6ECHO_H */
