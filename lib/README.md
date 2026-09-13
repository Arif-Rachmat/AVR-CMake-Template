# Libraries

The `lib/` directory is where you place AVR libraries that you want to use in your application.

The template is designed so that you **do not need to edit the main `CMakeLists.txt` when adding a library**.

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

The template automatically scans `lib/`, finds libraries that contain a `CMakeLists.txt`, adds them to the build, and links them to the application.

There is **no need to manually add `add_subdirectory()` or `target_link_libraries()`** to the application's `CMakeLists.txt`.

## Library naming rule

For automatic linking to work, a library must follow this convention:

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

It should also expose its public headers:

```cmake
target_include_directories(avr-scheduler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Then an application can simply do:

```cpp
#include <avr_sched.h>
```

and rebuild.

## Adding multiple libraries

You can clone as many libraries as needed:

```text
lib/
├── avr-scheduler/
├── avr-gpio/
├── avr-uart/
└── avr-display/
```

Each library is discovered automatically as long as it contains a `CMakeLists.txt` and follows the naming convention above.

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

## Library development

This same template can be used to develop a library.

Configure the repository in library mode:

```bash
cmake -B build \
  -DAVR_PROJECT_TYPE=LIBRARY
```

Then build it:

```bash
cmake --build build
```

In library mode, `src/main.c` and `src/main.cpp` are ignored by the library target. This lets you keep a temporary development program in `src/` without accidentally putting it into the library.

For permanent demonstration programs, use an `examples/` directory:

```text
avr-example-library/
├── CMakeLists.txt
├── include/
│   └── avr_example.h
├── src/
│   ├── avr_example.c
│   └── main.cpp          # optional development program
├── examples/
│   ├── CMakeLists.txt
│   └── basic/
│       └── main.cpp
└── README.md
```

Build the examples with:

```bash
cmake -B build \
  -DAVR_PROJECT_TYPE=LIBRARY \
  -DAVR_BUILD_EXAMPLES=ON

cmake --build build
```

Examples are separate executables and are not part of the library target itself.

## Beginner workflow

The intended workflow is simply:

```text
1. Clone the AVR-CMake-Template
2. Clone libraries into lib/
3. Write your application in src/main.cpp
4. Run cmake -B build
5. Run cmake --build build
```

The goal is to make adding an AVR library feel like adding a folder rather than modifying build-system code.