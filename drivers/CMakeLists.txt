add_library(drivers STATIC "")

target_sources(drivers
        PRIVATE
        crypto/aes128.c
        crypto/aes128.h
        crypto/ccms.c
        crypto/ccms.h
        crypto/hkdf.c
        crypto/hmac.c
        crypto/sha-private.h
        crypto/sha.h
        crypto/sha224-256.c
        crypto/usha.c
        hdlc.c
        hdlc.h
        sensor.c
        sensor.h
        serial.c
        serial.h
        timers.c
        timers.h)

target_compile_features(drivers PUBLIC c_std_99)
target_include_directories(drivers
        PUBLIC "."
        PUBLIC "crypto"
        PRIVATE "${CMAKE_SOURCE_DIR}/stack/02a-MAClow"
        PRIVATE "${CMAKE_SOURCE_DIR}/stack/02b-MAChigh"
        PRIVATE "${CMAKE_SOURCE_DIR}/stack/03a-IPHC"
        PRIVATE "${CMAKE_SOURCE_DIR}/stack/03b-IPv6"
        PRIVATE "${CMAKE_SOURCE_DIR}/stack/cross-layers")

target_link_libraries(drivers PUBLIC bsp)
target_link_libraries(drivers PUBLIC kernel)
