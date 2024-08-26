#include "6502_rom_bin.h"

// #define SERIAL_DEBUG

#define CLK_DELAY_US 0

#define RESB PIN_PE0
#define CLK PIN_PE1
#define RWB PIN_PE2

#define DATA_BUS VPORTA
#define ADDR_LOW VPORTD
#define ADDR_HIGH VPORTF
#define A15 PIN_PE3
#define VIA_PORT PORTC

#define VIA_ADDR 0x4000
#define UART_RX 0x5000
#define UART_TX 0x5001
#define UART_RX_RDY 0x5002

#define RAM_SIZE 0x1000

// "RAM" at 0x0000
uint8_t RAM[RAM_SIZE] = {0};

uint16_t addr = 0;
uint8_t data = 0;
bool rw = 0;
uint32_t cycle = 0;

bool program_mode = false;
uint8_t special_count = 0;
uint8_t serial_char = 0;

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

  Serial1.begin(115200);

  delay(100);

#ifdef SERIAL_DEBUG
  Serial1.println("\r\nStart!");
#endif

  // Take 6502 out of reset
  digitalWriteFast(RESB, HIGH);
}

void loop() {
#ifdef SERIAL_DEBUG
  Serial1.print(cycle);
  Serial1.print(" | ");
#endif

  // PHI goes LOW
  digitalWriteFast(CLK, LOW);
  delayMicroseconds(CLK_DELAY_US);

  rw = digitalReadFast(RWB);
  addr = (digitalReadFast(A15) << 15) | (ADDR_HIGH.IN << 8) | ADDR_LOW.IN;

  if (rw) {
    // 6502 is reading from memory
    DATA_BUS.DIR = 0xFF;

    if (addr >= (0x10000 - ROM_SIZE)) {
      // Read from "ROM"
      DATA_BUS.OUT = ROM[addr % ROM_SIZE];
    }
    else if (addr < RAM_SIZE) {
      // Read from "RAM"
      DATA_BUS.OUT = RAM[addr % RAM_SIZE];
    }
    else if (addr == UART_RX_RDY) {
      DATA_BUS.OUT = (Serial1.available() > 0 || serial_char != 0) ? 0x01 : 0x00;
    }
    else if (addr == UART_RX) {
      if (serial_char != 0) {
        DATA_BUS.OUT = serial_char;
        serial_char = 0;
      }
      else {
        DATA_BUS.OUT = Serial1.read();
      }
    }
  }
  else {
    // 6502 is writing to memory
    DATA_BUS.DIR = 0x00;
  }

  // PHI goes HIGH
  digitalWriteFast(CLK, HIGH);
  delayMicroseconds(CLK_DELAY_US);

  // Read the data bus
  data = DATA_BUS.IN;

  if (rw) {
#ifdef SERIAL_DEBUG
    Serial1.print("R");
#endif
  }
  else {
#ifdef SERIAL_DEBUG
    Serial1.print("W");
#endif
    // Write data value to "RAM"
    if (addr < RAM_SIZE) {
      RAM[addr] = data;
    }
    else if (addr == VIA_ADDR) {
      // Write byte to "VIA"
      VIA_PORT.OUT = ((data & 0xF) << 2);
    }
    else if (addr == UART_TX) {
      Serial1.write(data);
    }
  }

#ifdef SERIAL_DEBUG
  Serial1.print(" | A: 0x");
  Serial1.print(addr, HEX);
  Serial1.print(" | D: 0x");
  Serial1.print(data, HEX);
  Serial1.println();
#endif

  cycle++;
}
