# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(lumadoub-firmware C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(lumadoub-firmware lumadoub-firmware.c )

pico_set_program_name(lumadoub-firmware "lumadoub-firmware")
pico_set_program_version(lumadoub-firmware "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(lumadoub-firmware 0)
pico_enable_stdio_usb(lumadoub-firmware 1)

# Add the standard library to the build
target_link_libraries(lumadoub-firmware
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(lumadoub-firmware PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(lumadoub-firmware 
        hardware_i2c
        )

pico_add_extra_outputs(lumadoub-firmware)

