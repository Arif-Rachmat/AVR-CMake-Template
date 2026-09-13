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

# Use the repository directory name as the CMake project/target name.
# This keeps independently cloned libraries from sharing a generic target name.
get_filename_component(PROJECT_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" PROJECT_NAME "${PROJECT_NAME}")
project(${PROJECT_NAME} C CXX)

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

    # Automatically discover every library cloned into lib/.
    # Each library is expected to use its directory name as its CMake target name.
    file(GLOB LIB_DIRECTORIES CONFIGURE_DEPENDS LIST_DIRECTORIES true
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/*"
    )

    set(AVR_AUTO_LIBRARIES)

    foreach(LIB_DIR IN LISTS LIB_DIRECTORIES)
        if(IS_DIRECTORY "${LIB_DIR}" AND EXISTS "${LIB_DIR}/CMakeLists.txt")
            get_filename_component(LIB_NAME "${LIB_DIR}" NAME)

            message(STATUS "Adding AVR library: ${LIB_NAME}")
            add_subdirectory(
                "${LIB_DIR}"
                "${CMAKE_BINARY_DIR}/lib/${LIB_NAME}"
            )

            if(TARGET ${LIB_NAME})
                list(APPEND AVR_AUTO_LIBRARIES ${LIB_NAME})
            else()
                message(WARNING
                    "Library '${LIB_NAME}' was found in lib/ but did not create "
                    "a CMake target named '${LIB_NAME}'. It will not be linked automatically."
                )
            endif()
        endif()
    endforeach()

    if(AVR_AUTO_LIBRARIES)
        target_link_libraries(${PROJECT_NAME}.elf
            PRIVATE
                ${AVR_AUTO_LIBRARIES}
        )
    endif()

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
