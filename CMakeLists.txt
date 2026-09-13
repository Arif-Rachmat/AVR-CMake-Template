cmake_minimum_required(VERSION 3.16)

# Configuration Variables
set(MCU atmega328p CACHE STRING "Target microcontroller")
set(F_CPU 16000000UL CACHE STRING "Clock frequency in Hz")
set(AVRDUDE_PROGRAMMER arduino CACHE STRING "avrdude programmer")
set(AVRDUDE_PORT COM5 CACHE STRING "avrdude serial port")

# Project mode
set(AVR_PROJECT_TYPE APPLICATION CACHE STRING "Project type")
set(AVR_BUILD_EXAMPLES OFF CACHE BOOL "Build library examples")
set_property(CACHE AVR_PROJECT_TYPE PROPERTY STRINGS APPLICATION LIBRARY)

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
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.c"
    "src/*.cpp"
)

if(AVR_PROJECT_TYPE STREQUAL "LIBRARY")
    # Library mode: src/main.c/cpp is development-only and is not compiled.
    list(FILTER SOURCES EXCLUDE REGEX "/main\\.(c|cpp)$")

    if(SOURCES)
        add_library(${PROJECT_NAME} STATIC ${SOURCES})
    else()
        add_library(${PROJECT_NAME} INTERFACE)
    endif()

    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    # Optional examples are separate executables.
    if(AVR_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
        add_subdirectory(examples)
    endif()

else()
    # Application mode: src/main.cpp/main.c remains the normal entry point.
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
endif()
