#include "vr65c02.h"
#include <peekpoke.h>

#pragma warn(unused-param, push, off)
int write(int fd, const char *buf, unsigned count) {
    int i = 0;
    for (i; i < count; i++) {
        POKE(UART_TX, buf[i]);
    }

    return i;
}
#pragma warn(unused-param, pop)

char serial_getc() {
    while (PEEK(UART_RX_RDY) == 0) {
    }
    return PEEK(UART_RX);
}

void serial_putc(char c) { POKE(UART_TX, c); }
