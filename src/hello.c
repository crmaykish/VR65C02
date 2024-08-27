#include "vr65c02.h"
#include <stdio.h>

int main() {
    // unsigned int a = 0;

    printf("\r\nHello!\r\nEnter some letters: \r\n");

    while (1) {
        printf("You entered: %c\r\n", uart_getc());
        // printf("Hello! 0x%04X\r\n", a);
        // a++;
    }

    return 0;
}
