#include "config.h"
#include "defs.h"
#include "queue.h"
#include "serial.h"
#include "packetfunctions.h"
#include "IEEE802154E.h"
#include "radio.h"
#include "IEEE802154_security.h"
#include "sixtop.h"

//=========================== defination =====================================

#define HIGH_PRIORITY_QUEUE_ENTRY 5

//=========================== variables =======================================

queue_vars_t queue_vars;

//=========================== prototypes ======================================

void queue_reset_entry(QueueEntry_t *entry);

#if OPENWSN_6LO_FRAGMENTATION_C
void queue_reset_big_entry(QueueBigEntry_t *entry);
#endif

//=========================== public ==========================================

//======= admin

/**
\brief Initialize this module.
*/
void queue_init() {
    uint8_t i;
    for (i = 0; i < QUEUE_LENGTH; i++) {
        queue_reset_entry(&(queue_vars.queue[i]));
    }

#if OPENWSN_6LO_FRAGMENTATION_C
    for (i = 0; i < BIG_QUEUE_LENGTH; i++) {
        queue_reset_big_entry(&(queue_vars.big_queue[i]));
    }
#endif
}

/**
\brief Trigger this module to print status information, over serial.

debugPrint_* functions are used by the openserial module to continuously print
status information about several modules in the OpenWSN stack.

\returns TRUE if this function printed something, FALSE otherwise.
*/
bool debugPrint_queue() {
    debug_QueueEntry_t output[QUEUE_LENGTH];
    uint8_t i;
    for (i = 0; i < QUEUE_LENGTH; i++) {
        output[i].creator = queue_vars.queue[i].creator;
        output[i].owner = queue_vars.queue[i].owner;
    }
    openserial_printStatus(STATUS_QUEUE, (uint8_t * ) & output, QUEUE_LENGTH * sizeof(debug_QueueEntry_t));
    return TRUE;
}

//======= called by any component

/**
\brief Request a new (free) packet buffer.

Component throughout the protocol stack can call this function is they want to
get a new packet buffer to start creating a new packet.

\note Once a packet has been allocated, it is up to the creator of the packet
      to free it using the queue_freePacketBuffer() function.

\returns A pointer to the queue entry when it could be allocated, or NULL when
         it could not be allocated (buffer full or not synchronized).
*/
QueueEntry_t* queue_getFreePacketBuffer(uint8_t creator) {
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    // refuse to allocate if we're not in sync
    if (ieee154e_isSynch() == FALSE && creator > COMPONENT_IEEE802154E) {
        ENABLE_INTERRUPTS();
        return NULL;
    }

    // if you get here, I will try to allocate a buffer for you

    // if there is no space left for high priority queue, don't reserve
    if (queue_isHighPriorityEntryEnough() == FALSE && creator > COMPONENT_SIXTOP_RES) {
        ENABLE_INTERRUPTS();
        return NULL;
    }

    // walk through queue and find free entry
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].owner == COMPONENT_NULL) {
            queue_vars.queue[i].creator = creator;
            queue_vars.queue[i].owner = COMPONENT_OPENQUEUE;
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }
    ENABLE_INTERRUPTS();
    return NULL;
}

/**
\brief Free a previously-allocated packet buffer.

\param pkt A pointer to the previsouly-allocated packet buffer.

\returns E_SUCCESS when the freeing was succeful.
\returns E_FAIL when the module could not find the specified packet buffer.
*/
owerror_t queue_freePacketBuffer(QueueEntry_t *pkt) {
    uint8_t i;

    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

#if OPENWSN_6LO_FRAGMENTATION_C
    if (pkt->is_big_packet) {
        for (i = 0; i < BIG_QUEUE_LENGTH; i++) {
            if ((QueueBigEntry_t *) pkt == &queue_vars.big_queue[i]) {
                if (queue_vars.big_queue[i].standard_entry.owner == COMPONENT_NULL) {
                    // log the error
                    LOG_CRITICAL(COMPONENT_OPENQUEUE, ERR_FREEING_UNUSED,(errorparameter_t) 0,(errorparameter_t) 0);
                }

                queue_reset_big_entry((QueueBigEntry_t *) pkt);
                ENABLE_INTERRUPTS();
                return E_SUCCESS;
            }
        }
    } else {
#endif
        for (i = 0; i < QUEUE_LENGTH; i++) {
            if (&queue_vars.queue[i] == pkt) {
                if (queue_vars.queue[i].owner == COMPONENT_NULL) {
                    // log the error
                    LOG_CRITICAL(COMPONENT_OPENQUEUE, ERR_FREEING_UNUSED, (errorparameter_t) 0, (errorparameter_t) 0);
                }
                queue_reset_entry(&(queue_vars.queue[i]));
                ENABLE_INTERRUPTS();
                return E_SUCCESS;
            }
        }
#if OPENWSN_6LO_FRAGMENTATION_C
    }
#endif

    // log the error
    LOG_CRITICAL(COMPONENT_OPENQUEUE, ERR_FREEING_ERROR, (errorparameter_t) 0, (errorparameter_t) 0);
    ENABLE_INTERRUPTS();
    return E_FAIL;
}

#if OPENWSN_6LO_FRAGMENTATION_C
QueueEntry_t* queue_getFreeBigPacketBuffer(uint8_t creator) {
    uint8_t i;

    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    // refuse to allocate if we're not in sync
    if (ieee154e_isSynch() == FALSE) {
        ENABLE_INTERRUPTS();
        return NULL;
    }

    for (i = 0; i < BIG_QUEUE_LENGTH; i++) {
        if (queue_vars.big_queue[i].standard_entry.owner == COMPONENT_NULL) {

            queue_vars.big_queue[i].standard_entry.creator = creator;
            queue_vars.big_queue[i].standard_entry.owner = COMPONENT_OPENQUEUE;
            queue_vars.big_queue[i].standard_entry.is_big_packet = TRUE;

            ENABLE_INTERRUPTS();
            return &queue_vars.big_queue[i].standard_entry;

        }
    }

    ENABLE_INTERRUPTS();
    return NULL;
}
#endif

/**
\brief Free all the packet buffers created by a specific module.

\param creator The identifier of the component, taken in COMPONENT_*.
*/
void queue_removeAllCreatedBy(uint8_t creator) {
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].creator == creator &&
                queue_vars.queue[i].owner != COMPONENT_IEEE802154E
                ) {
            queue_reset_entry(&(queue_vars.queue[i]));
        }
    }

#if OPENWSN_6LO_FRAGMENTATION_C
    for (i = 0; i < BIG_QUEUE_LENGTH; i++) {
        if (queue_vars.big_queue[i].standard_entry.creator == creator) {
            queue_reset_big_entry(&(queue_vars.big_queue[i]));
        }
    }
#endif

    ENABLE_INTERRUPTS();
}

//======= called by RES

QueueEntry_t* queue_sixtopGetSentPacket() {
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].owner == COMPONENT_IEEE802154E_TO_SIXTOP &&
            queue_vars.queue[i].creator != COMPONENT_IEEE802154E) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }

#if OPENWSN_6LO_FRAGMENTATION_C
    for (i = 0; i < BIG_QUEUE_LENGTH; i++) {
        if (((QueueEntry_t*)&queue_vars.big_queue[i])->owner == COMPONENT_IEEE802154E_TO_SIXTOP &&
            ((QueueEntry_t*)&queue_vars.big_queue[i])->creator != COMPONENT_IEEE802154E) {
            ENABLE_INTERRUPTS();
            return (QueueEntry_t*)&queue_vars.big_queue[i];
        }
    }
#endif

    ENABLE_INTERRUPTS();
    return NULL;
}

QueueEntry_t* queue_sixtopGetReceivedPacket() {
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].owner == COMPONENT_IEEE802154E_TO_SIXTOP &&
            queue_vars.queue[i].creator == COMPONENT_IEEE802154E) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }
    ENABLE_INTERRUPTS();
    return NULL;
}

uint8_t queue_getNum6PReq(open_addr_t *neighbor) {

    uint8_t i;
    uint8_t num6Prequest;

    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    num6Prequest = 0;
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                queue_vars.queue[i].creator == COMPONENT_SIXTOP_RES &&
                queue_vars.queue[i].l2_sixtop_messageType == SIXTOP_CELL_REQUEST &&
                packetfunctions_sameAddress(neighbor, &queue_vars.queue[i].l2_nextORpreviousHop)
                ) {
            num6Prequest += 1;
        }
    }
    ENABLE_INTERRUPTS();
    return num6Prequest;
}

uint8_t queue_getNum6PResp() {

    uint8_t i;
    uint8_t num6Presponse;

    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    num6Presponse = 0;
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                queue_vars.queue[i].creator == COMPONENT_SIXTOP_RES &&
                queue_vars.queue[i].l2_sixtop_messageType == SIXTOP_CELL_RESPONSE
                ) {
            num6Presponse += 1;
        }
    }
    ENABLE_INTERRUPTS();
    return num6Presponse;
}

void queue_remove6PrequestToNeighbor(open_addr_t *neighbor) {

    uint8_t i;

    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                queue_vars.queue[i].creator == COMPONENT_SIXTOP_RES &&
                queue_vars.queue[i].l2_sixtop_messageType == SIXTOP_CELL_REQUEST &&
                packetfunctions_sameAddress(neighbor, &queue_vars.queue[i].l2_nextORpreviousHop)
                ) {
            queue_reset_entry(&(queue_vars.queue[i]));
        }
    }
    ENABLE_INTERRUPTS();
}

//======= called by IEEE80215E

bool queue_isHighPriorityEntryEnough() {
    uint8_t i;
    uint8_t numberOfEntry;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    numberOfEntry = 0;
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].creator > COMPONENT_SIXTOP_RES) {
            numberOfEntry++;
        }
    }

    if (numberOfEntry > QUEUE_LENGTH - HIGH_PRIORITY_QUEUE_ENTRY) {
        ENABLE_INTERRUPTS();
        return FALSE;
    } else {
        ENABLE_INTERRUPTS();
        return TRUE;
    }
}

QueueEntry_t* queue_macGetEBPacket() {
   uint8_t i;
   INTERRUPT_DECLARATION();
   DISABLE_INTERRUPTS();
   for (i=0; i < QUEUE_LENGTH; i++) {
      if (queue_vars.queue[i].owner==COMPONENT_SIXTOP_TO_IEEE802154E &&
          queue_vars.queue[i].creator==COMPONENT_SIXTOP              &&
          packetfunctions_isBroadcastMulticast(&(queue_vars.queue[i].l2_nextORpreviousHop))) {
         ENABLE_INTERRUPTS();
         return &queue_vars.queue[i];
      }
   }
   ENABLE_INTERRUPTS();
   return NULL;
}

QueueEntry_t* queue_macGetKaPacket(open_addr_t* toNeighbor) {
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
            queue_vars.queue[i].creator == COMPONENT_SIXTOP &&
            toNeighbor->type == ADDR_64B &&
            packetfunctions_sameAddress(toNeighbor, &queue_vars.queue[i].l2_nextORpreviousHop)
                ) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }
    ENABLE_INTERRUPTS();
    return NULL;
}

QueueEntry_t*  queue_macGetDIOPacket(){
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
            queue_vars.queue[i].creator == COMPONENT_ICMPv6RPL &&
            packetfunctions_isBroadcastMulticast(&(queue_vars.queue[i].l2_nextORpreviousHop))) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }
    ENABLE_INTERRUPTS();
    return NULL;
}

/**
\Brief replace the upstream packet nexthop payload by given newNextHop address
\param newNextHop.
*/
void queue_updateNextHopPayload(open_addr_t *newNextHop) {

    uint8_t i, j;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                (
                        newNextHop->type == ADDR_64B &&
                        packetfunctions_sameAddress(newNextHop, &queue_vars.queue[i].l2_nextORpreviousHop) == FALSE
                )
                ) {
            if (
                    queue_vars.queue[i].creator >= COMPONENT_FORWARDING &&
                    queue_vars.queue[i].l3_useSourceRouting == FALSE
                    ) {
                memcpy(&queue_vars.queue[i].l2_nextORpreviousHop, newNextHop, sizeof(open_addr_t));
                for (j = 0; j < 8; j++) {
                    *((uint8_t *) queue_vars.queue[i].l2_nextHop_payload + j) = newNextHop->addr_64b[j];
                }
            }
        }
    }

    ENABLE_INTERRUPTS();
}

QueueEntry_t*  queue_getPacketByComponent(uint8_t component) {
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (queue_vars.queue[i].owner == component) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }

    ENABLE_INTERRUPTS();
    return NULL;
}

QueueEntry_t*  queue_macGetUnicastPacket(open_addr_t* toNeighbor){
    uint8_t i;
    INTERRUPT_DECLARATION();
    DISABLE_INTERRUPTS();

    // first to look the sixtop RES packet
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                queue_vars.queue[i].creator == COMPONENT_SIXTOP_RES &&
                (
                        toNeighbor->type == ADDR_64B &&
                        packetfunctions_sameAddress(toNeighbor, &queue_vars.queue[i].l2_nextORpreviousHop)
                ) &&
                queue_vars.queue[i].l2_sixtop_messageType == SIXTOP_CELL_RESPONSE
                ) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }

    // if reach here, then looking for other unicast packets
    for (i = 0; i < QUEUE_LENGTH; i++) {
        if (
                queue_vars.queue[i].owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                (
                        toNeighbor->type == ADDR_64B &&
                        packetfunctions_sameAddress(toNeighbor, &queue_vars.queue[i].l2_nextORpreviousHop)
                )
                ) {
            ENABLE_INTERRUPTS();
            return &queue_vars.queue[i];
        }
    }

#if OPENWSN_6LO_FRAGMENTATION_C
    for (i = 0; i < BIG_QUEUE_LENGTH; i++) {
        if (
                ((QueueEntry_t*)&queue_vars.big_queue[i])->owner == COMPONENT_SIXTOP_TO_IEEE802154E &&
                (
                        toNeighbor->type == ADDR_64B &&
                        packetfunctions_sameAddress(toNeighbor, &((QueueEntry_t*)&queue_vars.big_queue[i])->l2_nextORpreviousHop)
                )
                ) {
            ENABLE_INTERRUPTS();
            return (QueueEntry_t*)&queue_vars.big_queue[i];
        }
    }
#endif
    ENABLE_INTERRUPTS();
    return NULL;
}


//=========================== private =========================================

void queue_reset_entry(QueueEntry_t *entry) {
    //admin
    entry->creator = COMPONENT_NULL;
    entry->owner = COMPONENT_NULL;

    // Footer is longer if security is used
    entry->payload = &(entry->packet[IEEE802154_FRAME_SIZE - LENGTH_CRC - IEEE802154_SECURITY_TAG_LEN]);
    entry->length = 0;
    entry->is_cjoin_response = FALSE;
#if OPENWSN_6LO_FRAGMENTATION_C
    entry->is_big_packet = FALSE;
#endif
    //l4
    entry->l4_protocol = IANA_UNDEFINED;
    entry->l4_protocol_compressed = FALSE;
    //l3
    entry->l3_destinationAdd.type = ADDR_NONE;
    entry->l3_sourceAdd.type = ADDR_NONE;
    entry->l3_useSourceRouting = FALSE;
#if OPENWSN_6LO_FRAGMENTATION_C
    entry->l3_isFragment = FALSE;
#endif
    //l2
    entry->l2_sixtop_command = IANA_6TOP_CMD_NONE;
    entry->l2_nextORpreviousHop.type = ADDR_NONE;
    entry->l2_frameType = IEEE154_TYPE_UNDEFINED;
    entry->l2_retriesLeft = 0;
    entry->l2_IEListPresent = 0;
    entry->l2_isNegativeACK = 0;
    entry->l2_payloadIEpresent = 0;
    entry->l2_sendOnTxCell = FALSE;
    //l2-security
    entry->l2_securityLevel = 0;
}

#if OPENWSN_6LO_FRAGMENTATION_C
void queue_reset_big_entry(QueueBigEntry_t *entry) {
    queue_reset_entry(&(entry->standard_entry));

    // reset pointer to the end op the extended buffer
    entry->standard_entry.payload = &(entry->standard_entry.packet[IPV6_PACKET_SIZE]);
}
#endif
