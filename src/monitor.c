#include "vr65c02.h"
#include <6502.h>
#include <peekpoke.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT ">"

#define INPUT_BUFFER_SIZE 32
#define COMMAND_MAX_LENGTH 6
#define MEMDUMP_BYTES 128

typedef struct {
    char name[6];
    char param1[6];
    char param2[6];
    char desc[48];
    void (*handler)();
} command_t;

void header();
void prompt();
void free_ram();
uint8_t readline(char *buffer);
void memdump(uint16_t address, uint8_t bytes);

void handler_help();
void handler_info();
void handler_peek();
void handler_poke();
void handler_dump();
void command_not_found(char *command_name);

// CPU type names
static const char *cpu_names[] = {"6502", "65C02",  "65816",
                                  "4510", "65SC02", "65CE02"};

static const command_t commands[] = {
    {"help", "", "", "Show this message", handler_help},
    {"info", "", "", "Get system info", handler_info},
    {"peek", "addr", "", "Peek at a memory address", handler_peek},
    {"poke", "addr", "val", "Poke a value into memory", handler_poke},
    {"dump", "addr", "", "Dump memory in hex and ASCII", handler_dump},
};

static const uint8_t COMMAND_COUNT = sizeof(commands) / sizeof(command_t);

int main() {
    uint8_t i;
    char buffer[INPUT_BUFFER_SIZE];
    char *command;
    bool command_handled;

    header();

    while (true) {
        command_handled = false;

        // Present the command prompt and wait for input
        prompt();
        readline(buffer);
        printf("\r\n");

        command = strtok(buffer, " ");

        // Look for the command name in the command list
        for (i = 0; i < COMMAND_COUNT; i++) {
            if (strncmp(command, commands[i].name, COMMAND_MAX_LENGTH) == 0) {
                // Found the command, handle it
                commands[i].handler();
                command_handled = true;
                break;
            }
        }

        if (!command_handled) {
            command_not_found(command);
        }

        printf("\r\n");
    }

    return 0;
}

void header() { printf("\r\n*** VR65C02 Monitor ***\r\n"); }

void prompt() { printf("%s ", PROMPT); }

void free_ram() { printf("%d bytes free", _heapmemavail()); }

void handler_help() {
    uint8_t i = 0;

    printf("Available commands:\r\n");

    for (i = 0; i < COMMAND_COUNT; i++) {
        printf(commands[i].name);

        if (commands[i].param1[0] != '\0') {
            printf(" <");
            printf(commands[i].param1);
            printf(">");
        }
        if (commands[i].param2[0] != '\0') {
            printf(" <");
            printf(commands[i].param2);
            printf(">");
        }

        printf(" : ");

        printf(commands[i].desc);

        if (i != COMMAND_COUNT - 1) {
            printf("\r\n");
        }
    }
}

void handler_info() {
    printf("CPU: ");
    printf(cpu_names[getcpu()]);
    printf("\r\nRAM: ");
    free_ram();
}

void handler_peek() {
    char *param1 = strtok(NULL, " ");
    uint16_t addr = strtol(param1, 0, 16);

    printf("%02X", PEEK(addr));
}

void handler_poke() {
    char *param1 = strtok(NULL, " ");
    char *param2 = strtok(NULL, " ");

    uint16_t addr = strtol(param1, 0, 16);
    uint8_t val = strtol(param2, 0, 16);

    POKE(addr, val);
    printf("OK");
}

void handler_dump() {
    char *param1 = strtok(NULL, " ");
    uint16_t addr = strtol(param1, 0, 16);

    memdump(addr, MEMDUMP_BYTES);
}

void print_string_bin(char *str, uint8_t max) {
    uint8_t i = 0;

    while (i < max) {
        if (str[i] >= 32 && str[i] < 127) {
            serial_putc(str[i]);
        } else {
            serial_putc('.');
        }

        i++;
    }
}

void memdump(uint16_t address, uint8_t bytes) {
    uint16_t i = 0;
    uint8_t b = 0;

    printf("%04X  ", address);

    while (i < bytes) {
        b = PEEK(address + i);
        printf("%02X ", b);

        i++;

        if (i % 16 == 0 && i < bytes) {
            printf(" |");
            print_string_bin((char *)(address + i - 16), 16);

            printf("|\r\n%04X  ", address + i);
        } else if (i % 8 == 0) {
            serial_putc(' ');
        }
    }

    serial_putc('|');
    print_string_bin((char *)(address + i - 16), 16);
    serial_putc('|');
}

void command_not_found(char *command_name) {
    printf("Command not found: %s", command_name);
}

uint8_t readline(char *buffer) {
    uint8_t count = 0;
    uint8_t in = serial_getc();

    while (in != '\n' && in != '\r') {
        // Character is printable ASCII
        if (in >= 0x20 && in < 0x7F) {
            serial_putc(in);

            buffer[count] = in;
            count++;
        }

        in = serial_getc();
    }

    buffer[count] = 0;

    return count;
}
