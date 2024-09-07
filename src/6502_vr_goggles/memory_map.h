#ifndef CONFIG_H_
#define CONFIG_H_

// Defining SOFT_RESET changes the function of GPIO pin 0 (PIN_PC0) to function as a reset button for the 6502
// Pulling GPIO pin 0 low will trigger the Arduino code to reset the CPU
// GPIO pin 0 is not usable for any other function while the soft reset function is enabled
#define SOFT_RESET

// RAM is mapped from 0x0000 to RAM_SIZE-1
#define RAM_SIZE 0x1400

// ROM is mapped from ROM_START to 0xFFFF
#define ROM_START 0xE000
#define ROM_SIZE (0x10000 - ROM_START)

// GPIO registers
#define VIA 0x7F00          // Read or write the four lowest bits to GPIO port
#define VIA_DIR 0x7F01      // Set the direction of the GPIO pins, 1 for output, 0 for input

// UART registers
#define UART_RX 0x7F10      // Read a byte from the UART
#define UART_TX 0x7F11      // Write a byte to the UART
#define UART_RX_RDY 0x7F12  // Check for incoming UART bytes

// I2C registers
#define I2C_CTRL 0x7F20     // Write 1 or 0 to activate or deactivate the I2C function of the GPIO pins
#define I2C_ADDR 0x7F21     // Write a byte to set the address of the active I2C peripheral
#define I2C_DATA 0x7F23     // Read or write a byte to the active I2C device

#endif
