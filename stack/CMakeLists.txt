add_library(stack STATIC "")

target_sources(stack
        PRIVATE
        02a-MAClow/adaptive_sync.c
        02a-MAClow/adaptive_sync.h
        02a-MAClow/IEEE802154.c
        02a-MAClow/IEEE802154.h
        02a-MAClow/IEEE802154_security.c
        02a-MAClow/IEEE802154_security.h
        02a-MAClow/IEEE802154E.c
        02a-MAClow/IEEE802154E.h
        02a-MAClow/topology.c
        02a-MAClow/topology.h
        02b-MAChigh/msf.c
        02b-MAChigh/msf.h
        02b-MAChigh/neighbors.c
        02b-MAChigh/neighbors.h
        02b-MAChigh/schedule.c
        02b-MAChigh/schedule.h
        02b-MAChigh/sixtop.c
        02b-MAChigh/sixtop.h
        03a-IPHC/frag.c
        03a-IPHC/frag.h
        03a-IPHC/iphc.c
        03a-IPHC/iphc.h
        03a-IPHC/bridge.c
        03a-IPHC/bridge.h
        03b-IPv6/forwarding.c
        03b-IPv6/forwarding.h
        03b-IPv6/icmpv6.c
        03b-IPv6/icmpv6.h
        03b-IPv6/icmpv6echo.c
        03b-IPv6/icmpv6echo.h
        03b-IPv6/icmpv6rpl.c
        03b-IPv6/icmpv6rpl.h
        04-TRAN/udp.c
        04-TRAN/udp.h
        04-TRAN/async.h
        04-TRAN/async_types.h
        04-TRAN/sock.h
        04-TRAN/sock.c
        04-TRAN/sock_internal.h
        04-TRAN/sock_types.h
        cross-layers/idmanager.c
        cross-layers/idmanager.h
        cross-layers/queue.c
        cross-layers/queue.h
        cross-layers/random.c
        cross-layers/random.h
        cross-layers/packetfunctions.c
        cross-layers/packetfunctions.h
        PUBLIC
        stack.c
        stack.h)

target_compile_features(stack PUBLIC c_std_99)
target_include_directories(stack
        PUBLIC "."
        PRIVATE "02a-MAClow"
        PRIVATE "02b-MAChigh"
        PRIVATE "03a-IPHC"
        PRIVATE "03b-IPv6"
        PRIVATE "04-TRAN"
        PRIVATE "cross-layers"
        INTERFACE "02a-MAClow"
        INTERFACE "02b-MAChigh"
        INTERFACE "03a-IPHC"
        INTERFACE "03b-IPv6"
        INTERFACE "04-TRAN"
        INTERFACE "cross-layers")

target_link_libraries(stack PUBLIC drivers)
target_link_libraries(stack PUBLIC kernel)
target_link_libraries(stack PUBLIC web)
target_link_libraries(stack PUBLIC apps)
