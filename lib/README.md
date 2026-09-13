# Libraries

This directory is for AVR libraries that are cloned into the project.

## Using a library

Clone a library repository into this directory:

```text
project/
├── CMakeLists.txt
├── src/
├── include/
└── lib/
    ├── README.md
    └── avr-example-library/
```

The library should provide its own `CMakeLists.txt` and define a CMake library target.

Add it to the main `CMakeLists.txt` with:

```cmake
add_subdirectory(lib/avr-example-library)

target_link_libraries(${PROJECT_NAME}.elf
    PRIVATE
        avr-example-library
)
```

Use the target name documented by the library if it differs from the directory name.

## Library development

This template can also be used as the starting point for developing a library.

Configure the repository in library mode:

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.toolchain.cmake \
  -DAVR_PROJECT_TYPE=LIBRARY
```

Build it with:

```bash
cmake --build build
```

In library mode, source files such as `src/main.cpp` are ignored by the library target. This allows a temporary development `main.cpp` to remain in the repository without becoming part of the library.

For permanent demonstration programs, put them under `examples/` and provide an `examples/CMakeLists.txt`. Enable them with:

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.toolchain.cmake \
  -DAVR_PROJECT_TYPE=LIBRARY \
  -DAVR_BUILD_EXAMPLES=ON
```

Examples are separate executables and are not part of the library itself.

## Recommended library structure

```text
avr-example-library/
├── CMakeLists.txt
├── include/
│   └── avr_example.h
├── src/
│   ├── avr_example.c
│   └── main.cpp          # optional development entry point
├── examples/
│   ├── CMakeLists.txt
│   └── basic/
│       └── main.cpp
└── README.md
```

A library should keep its public headers in `include/`, implementation files in `src/`, and demonstration programs in `examples/`.
