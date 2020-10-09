# build the oos_openwsn firmware

if (${BOARD} STREQUAL "python")
    add_subdirectory(${CMAKE_SOURCE_DIR}/bsp/boards/${BOARD})
else ()
    add_subdirectory(${CMAKE_SOURCE_DIR}/bsp/boards/${BOARD})
    add_subdirectory(${CMAKE_SOURCE_DIR}/drivers)
    add_subdirectory(${CMAKE_SOURCE_DIR}/apps)
    add_subdirectory(${CMAKE_SOURCE_DIR}/web)
    add_subdirectory(${CMAKE_SOURCE_DIR}/stack)
    add_subdirectory(${CMAKE_SOURCE_DIR}/kernel)
    add_subdirectory(${CMAKE_SOURCE_DIR}/projects/common/03oos_openwsn)
endif ()