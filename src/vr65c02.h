#ifndef VR65C02_H_
#define VR65C02_H_

#define UART_RX     0x5000
#define UART_TX     0x5001
#define UART_RX_RDY 0x5002

char uart_getc();

#endif
