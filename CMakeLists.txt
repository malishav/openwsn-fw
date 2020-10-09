cmake_minimum_required(VERSION 3.12)

project(OpenWSN-FW VERSION 1.0 LANGUAGES C ASM)
message(STATUS "openwsn-fw v${PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()

# specify all the build options
set(BOARD "" CACHE STRING "Select a target platform: openmote-cc2538, python, iot-lab_M3")
set(PROJECT "" CACHE STRING "Select a project: oos_openwsn")
set(BOOTLOAD "" CACHE STRING "Select ports to bootload, e.g., /dev/ttyUSBX")

# Force default build type to Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Choose the type of build, options are: Debug, Release, RelWithDebInfo and MinSizeRel." FORCE)
endif ()

include(options)

message(STATUS "Selected board: ${BOARD}")
message(STATUS "Building project: ${PROJECT}")

# include the generic definitions used by all libraries
include_directories(${CMAKE_SOURCE_DIR}/inc)

# include the projects

include(projects/${PROJECT})
