# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
if(EXISTS ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
    include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
elseif(DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else()
    set(PICO_SDK_PATH lib/pico-sdk)
endif()

if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
else()
    set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/lib/freertos)
endif()
message("FreeRTOS path: ${FREERTOS_KERNEL_PATH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This sets the variant of the board. We are using the pico_w
set(PICO_BOARD pico_w CACHE STRING "Board type")

# This imports the Pico SDK (must be included before the project)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# This imports the library configuration for FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# This defines your project name, you can name it anything you want.
project(lab11 C CXX ASM)

# These are some macros from the pico SDK that do some setup.
pico_sdk_init()

# This sets a preprocessor value for use in our code.
add_compile_definitions(
    PICO_ENTER_USB_BOOT_ON_EXIT=1
)

# This is where your project header files are located.
include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(server)
