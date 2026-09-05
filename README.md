# **AVR C/C++ Project Template (CMake)**

Flexible bare-metal development template for the AVR microcontroller family (ATmega, ATtiny, etc.). Built with **CMake**. seamlessly supports standalone or mixed **C** and **C++** development workflows. 

<div align="center">
    <img src="https://skillicons.dev/icons?i=c,cpp,cmake" alt="Languages"/>
</div>

Configured out-of-the-box for the **ATmega328P**, it can easily target any AVR chip by passing configuration variables during CMake setup.


---

## **Directory Structure 📂**

```text
├── CMakeLists.txt                # Main CMake configuration file
├── cmake/
│   └── avr-gcc.toolchain.cmake   # AVR cross-compilation toolchain file
├── include/                      # Header files (.h / .hpp)
├── src/                          # Source code (.c / .cpp)
│   └── main.cpp                  # Application entry point (or main.c)
└── README.md
```

---

## **Prerequisites❗**

Ensure the following tools are installed on your host system and available in your system `PATH`:

1. **AVR Toolchain**: `avr-gcc`, `avr-g++`, `avr-objcopy`, `avr-size`.
   * *Windows*: [Microchip AVR Toolchain](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers) or via MSYS2 / WinAVR.
   * *Linux*: `sudo apt install gcc-avr binutils-avr avr-libc`
   * *macOS*: `brew install avr-gcc`
2. **Programmer Utility**: [AVRDUDE](https://github.com/avrdudes/avrdude).
3. **Build System**: [CMake](https://cmake.org/download/) (v3.16+) and a build generator like [Ninja](https://ninja-build.org/) or GNU [Make].


---

## **Configuration Options⚙️**

Default parameters target the **ATmega328P** (e.g., Arduino Uno/Nano). You can override them inside `CMakeLists.txt` or pass them via `-D` flags during configuration:

| Option | Default Value | Description | Examples |
| --- | --- | --- | --- |
| `MCU` | `atmega328p` | Target microcontroller architecture. | `attiny85`, `atmega2560`, `atmega32u4` |
| `F_CPU` | `16000000UL` | Clock frequency in Hz. | `8000000UL`, `1000000UL` |
| `AVRDUDE_PROGRAMMER` | `arduino` | Programmer hardware driver. | `usbasp`, `wiring`, `avrispmkii`, `serialupdi` |
| `AVRDUDE_PORT` | `COM5` | Upload serial port. | `COM3`, `/dev/ttyUSB0`, `/dev/ttyACM0` |

---

## **Quick Starts🛠️**

Run these commands from the root directory of the project:

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

## **Editor Integration**

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

## To-Do List
- [ ] Add more built in editor support/configurations.
- [ ] Expand chip target presets and MCU frequency configurations in CMakeLists.txt.
- [ ] Add documentation for custom `avrdude` programmer types and baud rates.

---

## **License📜**

This project is licensed under the [MIT License](LICENSE) — free for both personal and commercial use.
