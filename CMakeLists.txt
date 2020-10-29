cmake_minimum_required(VERSION 3.12)

project(OpenWSN-FW VERSION 1.0 LANGUAGES C ASM)
message(STATUS "openwsn-fw v${PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()

if (POLICY CMP0057)
    cmake_policy(SET CMP0057 NEW)
endif ()

# list all platforms and boards
set(ALL_BOARDS openmote-cc2538 python iot-lab_M3)
set(ALL_PROJECTS oos_openwsn)

# specify all the build options
if (NOT BOARD)
    set(BOARD "python" CACHE STRING "Select a target platform: ${ALL_BOARDS}" FORCE)
endif ()

if (NOT PROJECT)
    set(PROJECT "oos_openwsn" CACHE STRING "Select a project: ${ALL_PROJECTS}" FORCE)
endif ()

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "" CACHE STRING "Set the toolchain file for cross-compiling" FORCE)
endif ()

set(BOOTLOAD "" CACHE STRING "Select ports to bootload, e.g., /dev/ttyUSBX")

if (NOT "${BOARD}" STREQUAL "python" AND "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    message(FATAL_ERROR "Cross-compiling requires an explicitly set toolchain, see cmake/toolchains/")
endif ()


if (${PROJECT} IN_LIST ALL_PROJECTS AND ${BOARD} IN_LIST ALL_BOARDS)
    message(STATUS "Selected board: ${BOARD}")
    message(STATUS "Building project: ${PROJECT}")
else ()
    message(FATAL_ERROR "Invalid board or project selected")
endif ()

# Force default build type to Debug unless already set (by an IDE or on the command line)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Choose the type of build, options are: Debug, Release, RelWithDebInfo and MinSizeRel." FORCE)
endif ()

# check for extra definitions
include(options)

# include the generic definitions used by all libraries
include_directories(${CMAKE_SOURCE_DIR}/inc)

# include the projects
include(projects/${PROJECT})
