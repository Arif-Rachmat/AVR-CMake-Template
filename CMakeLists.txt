cmake_minimum_required(VERSION 3.16)

# Configuration Variables
set(MCU atmega328p CACHE STRING "Target microcontroller")
set(F_CPU 16000000UL CACHE STRING "Clock frequency in Hz")
set(AVRDUDE_PROGRAMMER arduino CACHE STRING "avrdude programmer")
set(AVRDUDE_PORT COM5 CACHE STRING "avrdude serial port")

project(avr_app C CXX)

# Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json generation for LSP / clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Symlink compile_commands.json to the project root for LSP auto-detection
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json" AND NOT EXISTS "${CMAKE_SOURCE_DIR}/compile_commands.json")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
      "${CMAKE_BINARY_DIR}/compile_commands.json"
      "${CMAKE_SOURCE_DIR}/compile_commands.json"
  )
endif()

# Shared Optimization Flags
add_compile_options(
    -mmcu=${MCU}
    -DF_CPU=${F_CPU}
    -Os
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
)

# C++ Specific Bare-Metal Flags (Excludes exception/overhead bloat)
add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
)

# Linker flags to strip unused functions/data
add_link_options(-mmcu=${MCU} -Wl,--gc-sections)

# Auto-discover C and C++ source files
file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")

# Build target ELF
add_executable(${PROJECT_NAME}.elf ${SOURCES})
target_include_directories(${PROJECT_NAME}.elf PRIVATE include)

# Post-build: Extract HEX file
add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMENT "Extracting ${PROJECT_NAME}.hex..."
)

# Post-build: Print Flash and SRAM usage
add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} -t $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Memory Footprint:"
)

# Custom target: Upload via avrdude
add_custom_target(flash
    COMMAND avrdude -c ${AVRDUDE_PROGRAMMER} -P ${AVRDUDE_PORT} -p ${MCU} -U flash:w:${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex:i
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing firmware to ${MCU}..."
)
