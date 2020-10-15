set(OPENWSN_LOG_LEVEL "6" CACHE STRING "Select a logging level: 0 (no logs) - 6 (all logs)")
add_definitions(-DOPENWSN_LOG_LEVEL=${OPENWSN_LOG_LEVEL})

option(WEB_COAP "Enable the COAP protocol" OFF)
if (WEB_COAP)
    add_definitions(-DOPENWSN_COAP_C)
    set(OPENWSN_COAP_C "1")
else ()
    set(OPENWSN_COAP_C "0")
endif ()

option(STACK_UDP "Enable the UDP protocol" OFF)
if (STACK_UDP)
    add_definitions(-DOPENWSN_UDP_C)
    set(OPENWSN_UDP_C "1")
else ()
    set(OPENWSN_UDP_C "0")
endif ()

option(STACK_FRAG "Enable 6LoWPAN fragmentation" OFF)
if (STACK_FRAG)
    add_definitions(-DOPENWSN_6LO_FRAGMENTATION_C)
    set(OPENWSN_6LO_FRAGMENTATION_C "1")
else ()
    set(OPENWSN_6LO_FRAGMENTATION_C "0")
endif ()

option(STACK_PING "Enable the ping functionality (icmpv6_echo)" OFF)
if (STACK_PING)
    add_definitions(-DOPENWSN_ICMPV6ECHO_C)
    set(OPENWSN_ICMPV6ECHO_C "1")
else ()
    set(OPENWSN_ICMPV6ECHO_C "0")
endif ()

option(APP_CJOIN "Enable the ping functionality" OFF)
if (APP_CJOIN)
    add_definitions(-DOPENWSN_CJOIN_C)
    set(OPENWSN_CJOIN_C "1")
else ()
    set(OPENWSN_CJOIN_C "0")
endif ()

option(APP_UECHO "Enable the ping functionality" OFF)
if (APP_UECHO)
    add_definitions(-DOPENWSN_UECHO_C)
    set(OPENWSN_UECHO_C "1")
else ()
    set(OPENWSN_UECHO_C "0")
endif ()