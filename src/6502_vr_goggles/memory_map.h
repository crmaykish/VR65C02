#ifndef CONFIG_H_
#define CONFIG_H_

// Defining SOFT_RESET changes the function of GPIO pin 0 (PIN PC) to function as a reset button for the 6502
// Pulling GPIO pin 0 low and then high will trigger the Arduino code to reset the CPU
// GPIO pin 0 is not usable for any other function while the reset function is active
#define SOFT_RESET

// RAM is mapped from 0x0000 to RAM_SIZE-1
#define RAM_SIZE 0x1400

// ROM is mapped from ROM_START to 0xFFFF
#define ROM_START 0xE000
#define ROM_SIZE (0x10000 - ROM_START)

// GPIO registers
#define VIA 0x7F00
#define VIA_DIR 0x7F01

// UART registers
#define UART_RX 0x7F10
#define UART_TX 0x7F11
#define UART_RX_RDY 0x7F12

// I2C registers
#define I2C_CTRL 0x7F20
#define I2C_ADDR 0x7F21
#define I2C_DATA 0x7F23

#endif
