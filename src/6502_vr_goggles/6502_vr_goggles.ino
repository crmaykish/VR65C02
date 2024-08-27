#include "6502_rom_bin.h"

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

// Memory-mapped registers
#define VIA_ADDR 0x4000
#define UART_RX 0x5000
#define UART_TX 0x5001
#define UART_RX_RDY 0x5002

// RAM is mapped from 0x0000 to RAM_SIZE-1
#define RAM_SIZE 0x1000

// ROM is mapped from ROM_START to 0xFFFF
#define ROM_START 0xF000
#define ROM_SIZE (0x10000 - ROM_START)

// NOTE: ROM content is defined in the generted header file 6502_rom_bin.h

// Byte array acts as system RAM for the 6502
uint8_t RAM[RAM_SIZE] = {0};

uint16_t addr = 0;
uint8_t data = 0;
bool rw = 0;

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
    VIA_PORT.DIRSET = 0b00111100;
    VIA_PORT.OUT = 0b00000000;

    // TODO: alternate pin mapping for Serial 1 pins
    Serial1.begin(115200);

    // Toggle the clock a few times with the reset pin LOW
    for (int i = 0; i < 10; i++) {
        digitalWriteFast(CLK, LOW);
        delay(5);
        digitalWriteFast(CLK, HIGH);
        delay(5);
    }

    // Take 6502 out of reset
    digitalWriteFast(RESB, HIGH);
}

void loop() {
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
            DATA_BUS.OUT = Serial1.read();
        }
    } else {
        // 6502 is writing to memory
        DATA_BUS.DIR = 0x00;
    }

    // PHI goes HIGH
    digitalWriteFast(CLK, HIGH);
    // NOTE: The 6502 does not seem to require a delay on the rising edge
    // like it does on the falling edge of the clock

    if (!rw) {
        // Read the data bus
        data = DATA_BUS.IN;

        if (addr < RAM_SIZE) {
            // Write data bus to "RAM"
            RAM[addr] = data;
        } else if (addr == UART_TX) {
            // Write data bus to UART
            Serial1.write(data);
        } else if (addr == VIA_ADDR) {
            // Write data bus to "VIA"
            VIA_PORT.OUT =
                ((data & 0xF)
                 << 2); // TODO: can remove the shift with alt uart pins
        }
    }
}
