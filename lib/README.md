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

A library repository uses the same CMake template, but declares its purpose at the top of its committed `CMakeLists.txt`:

```cmake
set(AVR_PROJECT_TYPE LIBRARY)
```

The application template uses:

```cmake
set(AVR_PROJECT_TYPE APPLICATION)
```

This keeps the project type inside the repository itself. A consumer only needs to clone the library into `lib/` and build the application normally.

In library mode, the template creates a static library target instead of an executable and skips application-only steps such as HEX generation and the `flash` target.

A minimal library follows this structure:

```text
avr-example-library/
├── CMakeLists.txt
├── include/
│   └── avr_example.h
└── src/
    ├── avr_example.c
    └── main.cpp          # optional development program
```

The template automatically excludes `main.c` and `main.cpp` from the library target, so a development program may remain under `src/` without being linked into the library.

The library target exposes its public headers automatically:

```cmake
target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
)
```

The application can then simply use:

```cpp
#include <avr_example.h>
```

and rebuild.

### Project naming

The repository directory is used as the project name. Characters that are unsafe for a CMake target name are replaced with `_`.

For example:

```text
avr-scheduler       -> avr_scheduler
Base-Template (dev) -> Base_Template_dev
```

This means project directories may contain spaces, parentheses, or other characters without breaking the CMake configuration.

## Adding multiple libraries

You can clone as many libraries as needed:

```text
lib/
├── avr-scheduler/
├── avr-gpio/
├── avr-uart/
└── avr-display/
```

Each library is discovered automatically as long as it contains a `CMakeLists.txt` and uses `set(AVR_PROJECT_TYPE LIBRARY)`.

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
2. Keep AVR_PROJECT_TYPE as APPLICATION
3. Clone libraries into lib/
4. Write your application in src/main.cpp
5. Run cmake -B build
6. Run cmake --build build
```

### Library

```text
1. Start a repository from the AVR-CMake-Template
2. Change AVR_PROJECT_TYPE to LIBRARY
3. Put public headers in include/
4. Put library implementation files in src/
5. Keep an optional development main.cpp in src/ if useful
6. Put permanent demos in examples/
7. Run cmake -B build
8. Run cmake --build build
```

The goal is to keep one small, reusable CMake template while making the difference between an application and a library explicit in the repository itself.
