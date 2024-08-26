# VR65C02
A 65C02 computer system with an ATmega4809 simulating all of the memory and I/O. Two ICs are enough to run native 65C02 code on the real silicon. All of the 65C02's interactions with the outside world go through the microcontroller.

It's like the 65C02 is wearing the ATmega as a VR headset!

![Prototype](media/prototype.jpg)

## Background

### TL;DR

- Real 6502 code running on a real 6502
- ATmega4809 microcontroller emulates everything else
- 4K each of emulated ROM and RAM
- Memory-mapped serial port and GPIO register
- 6502 is programmed by uploading an Arduino sketch
- Eye-watering clock speed of 22kHz!

### Why?

Why not? This project started as a weekend experiment to see if I could run a 65C02 CPU without having to use a real ROM chip. That quickly turned into "Can I run 65C02 code on a real CPU without any physical memory or peripherals attached. The answer is yes.

This computer is not fast. It's not practical, but it is kind of fun.

### Why not a Raspberry Pi Pico or FPGA?

Those would both likely be better choices, but to be totally honest, the availability of the ATmega4809 in a 40 pin DIP package is what sold me. I like the aesthetic of two DIP ICs. That's really it.

## Features

![VR65C02 Block Diagram](media/block_diagram.png)

### Memory and I/O Emulation

VR65C02 is a fully functional computer system. The ATmega microcontroller provides emulated RAM and ROM as well as a passthrough UART and four GPIO pins that the 65C02 can access.

The microcontroller controls the RESET and CLOCK lines of the CPU, so all memory access is handled synchronously by the Arduino code. For each bus cycle, the microcontroller reads the address bus and R/W pin from the CPU. If the CPU is reading a memory address, the microcontroller will decode the address into a memory value or an I/O operation and write the appripriate value out to the data bus. On a write cycle, the microcontroller reads the data bus value into its simulated RAM or updates the appropriate I/O system. The microcontroller implements address decoding in code in a similar way to a traditional system using logic gates or PLDs.

Both RAM and ROM and backed by large byte arrays in the microcontroller's memory, the RAM using onboard SRAM and the ROM using onboard Flash.

The UART is a memory-mapped I/O device from the perspective of the 65C02, similar to a 6551 ACIA. The CPU can read and write bytes and the microcontroller passes them through to one of its native serial ports.

Similarly, there are four GPIO pins accessible as a memory-mapped register to the 65C02.

### Memory Map

The memory map of the 65C02 is as follows:
1. 4k "RAM" at 0x0000
2. GPIO register at 0x4000
3. UART registers at 0x5000
4. 4k "ROM" at 0xF000

The ATmega4809 has enough flash memory to support up to 32KB of ROM if necessary, but the total onboard SRAM is only 6KB.

## Software

The core of VR65C02 is the Arduino sketch running on the ATmega4809. As described above, this program simulates the entire bus cycle of the physical 65C02 CPU and all of the memory and I/O interactions.

Real 65C02 machine code is running on the CPU. This can be assembly or C code compiled with cc65. Since there is no physical ROM or RAM, the code has to live somewhere. My approach is to use the large amount of onboard Flash memory of the ATmega to store this. The main trick of VR65C02 is taking this compiled binary and getting it into the microcontroller Flash as part of the Arduino sketch upload.

The build process is done in a single script (build.sh). The process looks like this:
1. Compile 6502 C code into a binary ROM file.
2. Run a Python script to convert this binary file into a C header file with a large const array of bytes.
3. Compile the Arduino sketch with this generated header file.
4. Upload Arduino sketch to microcontroller with new 6502 code.

### Programming the ATmega (and by extension the 65C02)

The ATmega4809 is programmed via UPDI, specifically, I am using a standard FTDI USB-serial adapter (with a 4.7k resister between Rx and Tx) and the SerialUPDI programmer in the [MegaCoreX](https://github.com/MCUdude/MegaCoreX) Arduino board package. Since the 6502 code is uploaded as part of the Arduino sketch, it's almost like programming a 6502 as an Arduino.

## Limitations

All of the limitations of this system come down to hitting the limits of the ATmega4809 itself in one way or another.

The main drawback is obviously clock speed. Simulating the whole bus cycle and all of the potential memory and I/O operations of the 6502 on a 20 MHz 8-bit microcontroller is not exactly quick. Measuring the PHI2 output on the CPU with an oscilloscope shows that the effective clock speed is about 22 kHz. Even at this speed, VR65C02 is surprisingly usable for simple GPIO tasks or interacting over the serial port, but it's about 45x slower than a Commodore 64 or Apple II. There are undoubtedly optimizations to make in the Arduino code that could speed this up considerably, but even with perfect AVR assembly, I doubt this system could ever run in the MHz range.

Besides the slow speed, the other main limitations is the lack of available pins on the ATmega. All 31 available IO pins are being used to connect to the 65C02 bus and control signals. Two of the pins are used for the serial port and four are set up as GPIO. The GPIO pins could be used for other things, but there's not a huge number to spare.

In theory, these GPIO pins could be repurposed as chip-select signals to attach some real I/O devices to the system. It wouldn't be a stretch to attach a real RAM chip and have the microcontroller copy the program code from Flash into that RAM. In that scenario, a real clock oscillator could be used and the Arduino would become a slow peripheral using the RDY pin to handle the higher clock rate.

The ATmega4809 is also available in a 48 pin QFP package with quite a few more I/O pins. This might be fun to explore, but it would definitely detracts from the DIP aesthetic and make VR65C02 harder to build.
