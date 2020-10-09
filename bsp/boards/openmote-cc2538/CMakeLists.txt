add_library(bsp STATIC "")

file(GLOB BSP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/headers/*.h")
file(GLOB BSP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/../*.h")

target_sources(bsp
        PRIVATE
        ${BSP_HEADERS}
        ${BSP_SOURCES}
        adc_sensor.c
        adc_sensor.h
        board.c
        board_info.h
        cc2538rf.h
        cryptoengine.c
        debugpins.c
        eui64.c
        i2c.c
        leds.c
        pwm.c
        pwm.h
        radio.c
        sctimer.c
        sensors.c
        startup_gcc.c
        uart.c
        PUBLIC
        ${HEADERS})

target_include_directories(bsp
        PUBLIC ..
        PUBLIC .)
