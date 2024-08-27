#ifndef CONFIG_H_
#define CONFIG_H_

// RAM is mapped from 0x0000 to RAM_SIZE-1
#define RAM_SIZE 0x1400

// ROM is mapped from ROM_START to 0xFFFF
#define ROM_START 0xF000
#define ROM_SIZE (0x10000 - ROM_START)

// I/O Registers
#define VIA 0x7F00
#define UART_RX 0x7F10
#define UART_TX 0x7F11
#define UART_RX_RDY 0x7F12

#endif
