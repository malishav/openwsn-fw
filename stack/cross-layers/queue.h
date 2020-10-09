#ifndef OPENWSN_QUEUE_H
#define OPENWSN_QUEUE_H

/**
   \addtogroup cross-layers
 \{
   \addtogroup Queue
 \{
 */

#include "config.h"
#include "defs.h"

/* =========================== define ========================================== */

#ifndef PACKETQUEUE_LENGTH
#define QUEUE_LENGTH  (20)
#else
#define QUEUE_LENGTH  (PACKETQUEUE_LENGTH)
#endif

#if OPENWSN_6LO_FRAGMENTATION_C
#define BIG_QUEUE_LENGTH  (MAX_NUM_BIGPKTS)
#else
#define BIG_QUEUE_LENGTH  (0)
#endif

/* =========================== typedef ========================================= */

typedef struct {
    uint8_t creator;
    uint8_t owner;
} debug_QueueEntry_t;

typedef struct {
    // admin
    uint8_t     creator;        // the component which called getFreePacketBuffer()
    uint8_t     owner;          // the component which currently owns the entry
    uint8_t*    payload;        // pointer to the start of the payload within 'packet'
    int16_t     length;         // length in bytes of the payload
    // l7
#if DEADLINE_OPTION
    uint16_t    max_delay;     // Max delay in milliseconds before which the packet should be delivered to the receiver
    bool        orgination_time_flag;
    bool        drop_flag;
#endif
    bool        is_cjoin_response;
#if OPENWSN_6LO_FRAGMENTATION_C
    bool        is_big_packet;
#endif

    // l4
    uint8_t     l4_protocol;                    // l4 protocol to be used
    bool        l4_protocol_compressed;         // is the l4 protocol header compressed?
    uint16_t    l4_sourcePortORicmpv6Type;      // l4 source port
    uint16_t    l4_destination_port;            // l4 destination port
    uint8_t*    l4_payload;                     // pointer to the start of the payload of l4 (used for retransmits)
    uint8_t     l4_length;                      // length of the payload of l4 (used for retransmits)

    // l3
    open_addr_t l3_destinationAdd;      // 128b IPv6 destination (down stack)
    open_addr_t l3_sourceAdd;           // 128b IPv6 source address
    bool        l3_useSourceRouting;    // TRUE when the packet goes downstream

#if OPENWSN_6LO_FRAGMENTATION_C
    bool        l3_isFragment;
#endif
    // l2
    owerror_t   l2_sendDoneError;                               // outcome of trying to send this packet
    open_addr_t l2_nextORpreviousHop;                           // 64b IEEE802.15.4 next (down stack) or previous (up)
                                                                // hop address
    uint8_t     l2_frameType;                                   // beacon, data, ack, cmd
    uint8_t     l2_dsn;                                         // sequence number of the received frame
    uint8_t     l2_retriesLeft;                                 // number Tx retries left before packet dropped (dropped
                                                                // when hits 0)
    uint8_t     l2_numTxAttempts;                               // number Tx attempts
    asn_t       l2_asn;                                         // at what ASN the packet was Tx'ed or Rx'ed
    uint8_t*    l2_payload;                                     // pointer to the start of the payload of l2 (used for
                                                                // MAC to fill in ASN in ADV)
    cellInfo_ht l2_sixtop_celllist_add [CELL_LIST_MAX_LEN];     // record celllist to be added and will be added when 6P
                                                                // response sendDone
    cellInfo_ht l2_sixtop_celllist_delete [CELL_LIST_MAX_LEN];  // record celllist to be removed and will be removed
                                                                // when 6P response sendDone
    uint16_t    l2_sixtop_frameID;                              // frameID in 6P message
    uint8_t     l2_sixtop_messageType;                          // indicating the sixtop message type
    uint8_t     l2_sixtop_command;                              // command of the received 6p request, recorded in 6p
                                                                // response
    uint8_t     l2_sixtop_cellOptions;                          // celloptions, used when 6p response senddone. (it's
                                                                // the same with cellOptions in 6p request but with TX
                                                                // and RX bits have been flipped)
    uint8_t     l2_sixtop_returnCode;                           // return code in 6P response
    uint8_t*    l2_ASNpayload;                                  // pointer to the ASN in EB
    uint8_t*    l2_nextHop_payload;                             // pointer to the nexthop address in frame
    uint8_t     l2_joinPriority;                                // the join priority received in EB
    bool        l2_IEListPresent;                               // did have IE field?
    bool        l2_payloadIEpresent;                            // did I have payload IE field
    bool        l2_joinPriorityPresent;
    bool        l2_isNegativeACK;                               // is the negative ACK?
    int16_t     l2_timeCorrection;                              // record the timeCorrection
    bool        l2_sendOnTxCell;                                // mark the frame is sent on txCell
    // layer-2 security
    uint8_t     l2_securityLevel;                               // the security level specified for the current frame
    uint8_t     l2_keyIdMode;                                   // the key Identifier mode specified for the current
                                                                // frame
    uint8_t     l2_keyIndex;                                    // the key Index specified for the current frame
    open_addr_t l2_keySource;                                   // the key Source specified for the current frame
    uint8_t     l2_authenticationLength;                        // the length of the authentication field
    uint8_t     commandFrameIdentifier;                         // used in case of Command Frames
    uint8_t*    l2_FrameCounter;                                // pointer to the FrameCounter in the MAC header
    // l1 (drivers)
    uint8_t     l1_txPower;                                     // power for packet to Tx at
    int8_t      l1_rssi;                                        // RSSI of received packet
    uint8_t     l1_lqi;                                         // LQI of received packet
    bool        l1_crc;                                         // did received packet pass CRC check?
    // the packet
    uint8_t     packet [IEEE802154_FRAME_SIZE];                 // 1B spi address, 1B length, 125B data, 2B CRC, 1B LQI
} QueueEntry_t;


#if OPENWSN_6LO_FRAGMENTATION_C
typedef struct {
    QueueEntry_t    standard_entry;
    uint8_t         packet_remainder [IPV6_PACKET_SIZE - IEEE802154_FRAME_SIZE];    // 127 bytes already allocated in
                                                                                    // the QueueEntry
} QueueBigEntry_t;
#endif

/* =========================== module variables ================================ */

typedef struct {
    QueueEntry_t    queue [QUEUE_LENGTH];
#if OPENWSN_6LO_FRAGMENTATION_C
    QueueBigEntry_t big_queue [BIG_QUEUE_LENGTH];
#endif
} queue_vars_t;

/* =========================== prototypes ====================================== */
// admin
void    queue_init(void);
bool    debugPrint_queue(void);

// called by any component
QueueEntry_t* queue_getFreePacketBuffer(uint8_t creator);

#if OPENWSN_6LO_FRAGMENTATION_C
QueueEntry_t* queue_getFreeBigPacketBuffer(uint8_t creator);

#endif
owerror_t   queue_freePacketBuffer(QueueEntry_t* pkt);
void        queue_removeAllCreatedBy(uint8_t creator);
bool        queue_isHighPriorityEntryEnough(void);

// called by ICMPv6
void queue_updateNextHopPayload(open_addr_t* newNextHop);

// called by res
QueueEntry_t*   queue_sixtopGetSentPacket(void);
QueueEntry_t*   queue_sixtopGetReceivedPacket(void);
uint8_t         queue_getNum6PResp(void);
uint8_t         queue_getNum6PReq(open_addr_t* neighbor);
void            queue_remove6PrequestToNeighbor(open_addr_t* neighbor);

// called by IEEE80215E
QueueEntry_t*   queue_macGetEBPacket(void);
QueueEntry_t*   queue_macGetKaPacket(open_addr_t* toNeighbor);
QueueEntry_t*   queue_macGetDIOPacket(void);
QueueEntry_t*   queue_macGetUnicastPacket(open_addr_t* toNeighbor);

// called by transport layer
QueueEntry_t* queue_getPacketByComponent(uint8_t component);
/**
 \}
 \}
 */

#endif /* OPENWSN_OPENQUEUE_H */
