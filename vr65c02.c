#include "vr65c02.h"
#include <peekpoke.h>

#pragma warn(unused-param, push, off)
// write() must be defined for printf() to work
int write(int fd, const char *buf, unsigned count)
{
    int i = 0;
    for (i; i < count; i++)
    {
        POKE(UART_TX, buf[i]);
    }

    return i;
}
#pragma warn(unused-param, pop)