# **AVR C/C++ Project Template (CMake)**

Flexible bare-metal development template for the AVR microcontroller family (ATmega, ATtiny, etc.). Built with **CMake**. seamlessly supports standalone or mixed **C** and **C++** development workflows. 

<div align="center">
    <img src="https://skillicons.dev/icons?i=c,cpp,cmake" alt="Languages"/>
</div>

Configured out-of-the-box for the **ATmega328P**, it can easily target any AVR chip by passing configuration variables during CMake setup.

---

## **Table of content 📋**
1. [Directory Structure](#directory-structure-)
2. [Prerequisites](#prerequisites)
3. [Quick Starts](#quick-starts️)
4. [Configuration Options](#configuration-options️)
5. [Library](#library)
6. [Editor Integrations](#editor-integration)
7. [ToDo List](#to-do-list)
8. [License](#license)

## **Directory Structure 📂**

```text
├── .clangd                         # Clangd configuration for code completion and analysis
├── .vscode/                        # VS Code configuration directory
├── .zed/                           # Zed editor configuration directory
├── cmake/
│   └── avr-gcc.toolchain.cmake     # AVR cross-compilation toolchain file
├── include/                        # Public header files (.h / .hpp)
├── lib/                            # Local libraries and reusable components
├── src/                            # Application source code (.c / .cpp)
│   └── main.cpp                    # Application entry point (or main.c)
├── CMakeLists.txt                  # Main CMake configuration file
└── README.md                       # Project documentation
```

---

## **Prerequisites❗**

Ensure the following tools are installed on your host system and available in your system `PATH`:

- **AVR Toolchain**: [Microchip AVR Toolchain](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers)
- **Programmer Utility**: [AVRDUDE](https://github.com/avrdudes/avrdude).
- **Build System**: [CMake](https://cmake.org/download/) (v3.16+) and a build generator like [Ninja](https://ninja-build.org/) or GNU [Make].

### 1. Windows (Recommended: use [MSYS2 UCRT64](https://www.msys2.org/))

Open the **MSYS2 UCRT64** terminal and run:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-avr-toolchain mingw-w64-ucrt-x86_64-avrdude mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-make
```

> **Note**: Ensure `C:\msys64\ucrt64\bin` is added to your Windows system `PATH` if invoking these commands outside the MSYS2 terminal.


### 2. Linux (Debian / Ubuntu)

Run the following command in your terminal:

```bash
sudo apt update && sudo apt install -y gcc-avr binutils-avr avr-libc avrdude cmake ninja-build make
```


### 3. macOS (Homebrew)

Run the following command in your terminal:

```bash
brew install avr-gcc avrdude cmake ninja
```

---

## **Quick Starts🛠️**

### 1. Clone the Repo

Clone the repository and navigate to the project directory:

```bash
git clone https://github.com/Arif-Rachmat/AVR-CMake-Template.git <YOUR_PROJECT_DIR>
cd <YOUR_PROJECT_DIR>
```

### 2. Configure the Project

**Standard Configuration (Default: ATmega328P @ 16MHz):**

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.toolchain.cmake
```

**Targeting a Different MCU (e.g., ATtiny85 @ 8MHz via USBasp):**

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.toolchain.cmake \
  -DMCU=attiny85 \
  -DF_CPU=8000000UL \
  -DAVRDUDE_PROGRAMMER=usbasp \
  -DAVRDUDE_PORT=usb
```

### 3. Compile Firmware

Build the executable (`.elf`), binary (`.hex`), and output memory usage:

```bash
cmake --build build
```

### 4. Flash Microcontroller

Upload the firmware to hardware using `avrdude`:

```bash
cmake --build build --target flash
```

### Cleaning Build Directory

Delete all compiled objects and generated output files:

```bash
cmake --build build --target clean
# Or simply remove the build folder: rm -rf build/
```

---

## **Configuration Options ⚙️**

Default parameters target the **ATmega328P** (e.g., Arduino Uno/Nano). You can override them inside `CMakeLists.txt` or pass them via `-D` flags during configuration:

| Option | Default Value | Description | Examples |
| --- | --- | --- | --- |
| `MCU` | `atmega328p` | Target microcontroller architecture. | `attiny85`, `atmega2560`, `atmega32u4` |
| `F_CPU` | `16000000UL` | Clock frequency in Hz. | `8000000UL`, `1000000UL` |
| `AVRDUDE_PROGRAMMER` | `arduino` | Programmer hardware driver. | `usbasp`, `wiring`, `avrispmkii`, `serialupdi` |
| `AVRDUDE_PORT` | `COM5` | Upload serial port. | `COM3`, `/dev/ttyUSB0`, `/dev/ttyACM0` |

---

## **Library 📚**

This template supports integrating reusable libraries through the `lib/` directory. Each library can be kept self-contained with its own source files, headers, and CMake configuration, or following the exact directory structure of this template . while the main project handles adding and linking it to the application.

A minimal workflow looks like this:

```text
lib/
└── my_library/
    ├── include/
    │   └── my_library.h
    ├── src/
    │   └── my_library.c
    └── CMakeLists.txt
```

Make sure the `CMakeLists` of the library expose the header and add the source as static library:

```cmake
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME}
   PUBLIC
      include
)
```

Or if using this template to build the library, simply set the `AVR_PROJECT_TYPE` variable at the top of `CMakeLists.txt` to `LIBRARY`:

```cmake
set(AVR_PROJECT_TYPE LIBRARY) 
```

The application can then use the library normally:

```cpp
#include <my_library.h>

int main()
{
    my_library_init();
}
```

This keeps reusable code separated from application-specific code while allowing everything to be built together as a single firmware project.

> For more information about library and building one go to the [lib README](/lib/README.md) page.

---

## **Editor Integration 🔗**

### Visual Studio Code (see [VSCode folder](.vscode/))
1. Install the **C/C++** (`ms-vscode.cpptools`) and **CMake Tools** (`ms-vscode.cmake-tools`) extensions.
2. Run **CMake: Configure** from the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`).
3. Build using <kbd>F7</kbd> and flash via the CMake side-bar target menu.

### Zed (see [Zed folder](.zed/))
1. Open the project folder in [**Zed**](https://zed.dev).
2. Run the setup task:
   - Open command palette (`Ctrl+Shift+P` / `Cmd+Shift+P`) and type `task: spawn`.
   - Select `CMake: Configure (AVR Toolchain)`.
3. Compile or flash the target:
   - Select `CMake: Build Firmware` to compile.
   - Select `AVR: Upload / Flash Firmware` to flash.
4. Code completion and register navigation (`<avr/io.h>`) are handled natively via `clangd`.

---

## **To-Do List 📌**
- [ ] Add more built in editor support/configurations.
- [ ] Add libraries repository links as the example of directly pulling and integrating a library.
- [ ] Expand chip target presets and MCU frequency configurations in `CMakeLists.txt`.
- [ ] Add documentation for custom `avrdude` programmer types and baud rates.

---

## **License 📜**

This project is licensed under the [MIT License](LICENSE) — free for both personal and commercial use.
