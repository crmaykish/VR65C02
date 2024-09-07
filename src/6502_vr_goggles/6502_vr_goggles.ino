#include "6502_rom_bin.h"
#include "memory_map.h"
#include <Wire.h>

// MCU pins
#ifdef SOFT_RESET
#define SOFT_RES_SW PIN_PC0
#endif

// CPU control pins
#define RESB PIN_PE1
#define CLK PIN_PE0
#define RWB PIN_PE2

// CPU buses
#define DATA_BUS VPORTA
#define ADDR_LOW VPORTD
#define ADDR_HIGH VPORTF
#define A15 PIN_PE3
#define VIA_PORT PORTC

// Byte array acts as system RAM for the 6502
uint8_t RAM[RAM_SIZE] = {0};

// NOTE: ROM content is defined in the generated header file 6502_rom_bin.h

uint16_t addr = 0;
uint8_t data = 0;
bool rw = 0;

uint8_t i2c_dev = 0;

void reset_6502() {
    digitalWriteFast(RESB, LOW);

    // Toggle the clock a few times with the reset pin LOW
    for (int i = 0; i < 50; i++) {
        digitalWriteFast(CLK, LOW);
        delay(2);
        digitalWriteFast(CLK, HIGH);
        delay(2);
    }

    // Take 6502 out of reset
    digitalWriteFast(RESB, HIGH);
}

void setup() {
    // Setup buses as input
    ADDR_LOW.DIR = 0;
    ADDR_HIGH.DIR = 0;
    DATA_BUS.DIR = 0;
    pinMode(A15, INPUT);

    // Setup CPU control pins
    pinMode(RWB, INPUT);
    pinMode(RESB, OUTPUT);
    pinMode(CLK, OUTPUT);
    digitalWriteFast(RESB, LOW);
    digitalWriteFast(CLK, HIGH);

    // Setup "VIA" pins
    VIA_PORT.DIRCLR = 0b00001111;

#ifdef SOFT_RESET
    pinMode(SOFT_RES_SW, INPUT);
#endif

    Serial1.swap(1);
    Serial1.begin(115200);

    reset_6502();
}

void loop() {
#ifdef SOFT_RESET
    if (!digitalReadFast(SOFT_RES_SW)) {
        reset_6502();
    }
#endif

    // PHI goes LOW
    digitalWriteFast(CLK, LOW);

    // Delay slightly, giving the 6502 time to react to the falling clock edge
    // before reading the buses
    asm("nop");
    asm("nop");

    rw = digitalReadFast(RWB);
    addr = (digitalReadFast(A15) << 15) | (ADDR_HIGH.IN << 8) | ADDR_LOW.IN;

    if (rw) {
        // 6502 is reading from memory
        DATA_BUS.DIR = 0xFF;

        if (addr >= ROM_START) {
            // Read from "ROM"
            DATA_BUS.OUT = ROM[addr % ROM_SIZE];
        } else if (addr < RAM_SIZE) {
            // Read from "RAM"
            DATA_BUS.OUT = RAM[addr];
        } else if (addr == UART_RX_RDY) {
            // Read from UART RX ready register
            DATA_BUS.OUT = (Serial1.available() > 0) ? 0x01 : 0x00;
        } else if (addr == UART_RX) {
            // Read from serial port
            DATA_BUS.OUT = Serial1.read();
        } else if (addr == VIA) {
            // Read from VIA GPIO
            DATA_BUS.OUT = VIA_PORT.IN & 0xF;
        } else if (addr == VIA_DIR) {
            // Read from VIA direction register
            DATA_BUS.OUT = VIA_PORT.DIR & 0xF;
        } else if (addr == I2C_DATA) {
            Wire.requestFrom(i2c_dev, 1);
            if (Wire.available()) {
                DATA_BUS.OUT = Wire.read();
            } else {
                DATA_BUS.OUT = 0;
            }
        } else if (addr == I2C_ADDR) {
            DATA_BUS.OUT = i2c_dev;
        }
    } else {
        // 6502 is writing to memory
        DATA_BUS.DIR = 0x00;
    }

    // PHI goes HIGH
    digitalWriteFast(CLK, HIGH);
    // NOTE: The 6502 does not seem to require a delay on the rising edge
    // of the clock like it does on the falling edge

    if (!rw) {
        // Read the data bus
        data = DATA_BUS.IN;

        if (addr < RAM_SIZE) {
            // Write data bus to "RAM"
            RAM[addr] = data;
        } else if (addr == UART_TX) {
            // Write data bus to UART
            Serial1.write(data);
        } else if (addr == VIA) {
            // Write data bus to "VIA"
            VIA_PORT.OUT = (data & 0xF);
        } else if (addr == VIA_DIR) {
            // Write to VIA GPIO direction register
#ifdef SOFT_RESET
            // If soft reset switch is active, ignore the lowest bit
            VIA_PORT.DIRCLR = 0b00001110;
            VIA_PORT.DIRSET = data & 0b00001110;
#else
            // Otherwise, use the lowest four bits of the port as GPIO
            VIA_PORT.DIRCLR = 0xF;
            VIA_PORT.DIRSET = data & 0xF;
#endif
        } else if (addr == I2C_CTRL) {
            // Turn on or off the I2C function of the GPIO pins
            if (data & 0b01) {
                Wire.swap(1);
                Wire.begin();
            } else {
                Wire.end();
            }
        } else if (addr == I2C_ADDR) {
            // Set I2C device address
            i2c_dev = data;
        } else if (addr == I2C_DATA) {
            // Write a byte to active I2C device
            Wire.beginTransmission(i2c_dev);
            Wire.write(data);
            Wire.endTransmission();
        }
    }
}
