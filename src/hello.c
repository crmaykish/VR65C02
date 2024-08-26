#include <stdio.h>
#include "vr65c02.h"

int main() {
    unsigned int a = 0;

    printf("\r\n");

    while(1) {
        printf("Hello! 0x%04X\r\n", a);
        a++;
    }

    return 0;
}
