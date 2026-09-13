# Libraries

The `lib/` directory is where you place AVR libraries that you want to use in your application.

The template automatically discovers libraries in this directory, so you **do not need to edit the main `CMakeLists.txt` when adding a library**.

## Using a library

Clone the library repository directly into `lib/`:

```bash
git clone https://github.com/OWNER/avr-example-library.git lib/avr-example-library
```

Your project will then look like:

```text
project/
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── include/
└── lib/
    ├── README.md
    └── avr-example-library/
        ├── CMakeLists.txt
        ├── include/
        └── src/
```

Now configure and build normally:

```bash
cmake -B build
cmake --build build
```

The template scans `lib/`, finds directories containing a `CMakeLists.txt`, adds them to the build, and links their library targets to the application.

There is **no need to manually add `add_subdirectory()` or `target_link_libraries()`** to the application's `CMakeLists.txt`.

## Library repository convention

A library repository must declare itself as a library in its **committed `CMakeLists.txt`**. Project type should not be selected through a developer's local CMake command, because consumers clone the repository and need its build definition to be self-contained.

A minimal library can use:

```cmake
cmake_minimum_required(VERSION 3.16)
project(avr-example-library C)

add_library(avr-example-library STATIC
    src/avr_example.c
)

target_include_directories(avr-example-library
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

The important convention is:

```text
Library directory name = CMake target name
```

For example:

```text
lib/
└── avr-scheduler/
```

The library must create a target named `avr-scheduler`:

```cmake
add_library(avr-scheduler STATIC
    src/avr_sched.c
)
```

It should expose its public headers through the same target:

```cmake
target_include_directories(avr-scheduler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

The application can then simply use:

```cpp
#include <avr_sched.h>
```

and rebuild.

### Development `main.cpp`

A library may contain a development program in `src/main.cpp` while keeping it committed to the repository. This is safe as long as the library's `CMakeLists.txt` explicitly lists only library source files in `add_library()`.

For example:

```text
avr-example-library/
├── CMakeLists.txt
├── include/
│   └── avr_example.h
└── src/
    ├── avr_example.c
    └── main.cpp          # development program; not part of the library target
```

Do **not** use an unrestricted `file(GLOB_RECURSE ...)` over `src/` to construct a library target, because that can accidentally add `main.cpp` to the library.

For permanent demonstration programs, use an `examples/` directory instead.

## Adding multiple libraries

You can clone as many libraries as needed:

```text
lib/
├── avr-scheduler/
├── avr-gpio/
├── avr-uart/
└── avr-display/
```

Each library is discovered automatically as long as it contains a `CMakeLists.txt` and follows the target naming convention above.

For example:

```bash
git clone https://github.com/OWNER/avr-scheduler.git lib/avr-scheduler
git clone https://github.com/OWNER/avr-uart.git lib/avr-uart
```

Then:

```bash
cmake -B build
cmake --build build
```

No application CMake changes are necessary.

## Library examples

Permanent demonstration programs should live under `examples/` rather than being included in the library target:

```text
avr-example-library/
├── CMakeLists.txt
├── include/
├── src/
│   └── avr_example.c
└── examples/
    ├── CMakeLists.txt
    └── basic/
        └── main.cpp
```

Examples should be separate executable targets. A library may provide an option such as `AVR_BUILD_EXAMPLES` in its own `CMakeLists.txt` to control whether they are built during standalone library development.

When the library is consumed from an application, its normal library target is what the template links automatically; examples should not become part of the application automatically.

## Beginner workflow

### Application

```text
1. Clone the AVR-CMake-Template
2. Clone libraries into lib/
3. Write your application in src/main.cpp
4. Run cmake -B build
5. Run cmake --build build
```

### Library

```text
1. Start a library repository using the library CMake convention
2. Put public headers in include/
3. Put library implementation files in src/
4. Declare the library target explicitly with add_library()
5. Keep development programs out of the library target
6. Put permanent demos in examples/
7. Run cmake -B build
8. Run cmake --build build
```

The goal is to make library consumption feel like adding a folder rather than modifying application build-system code, while keeping each library's own build definition explicit and reproducible.
