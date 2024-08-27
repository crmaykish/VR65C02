CC = cl65
CA = ca65
AR = ar65

CFLAGS = -O -t none --cpu 65c02 --static-locals -I include

ROM_CFG = mem.cfg

ROM_LIB = rom.lib

HELLO_SRC = hello.c vr65c02.c

.PHONY: all clean

all: hello

# Firmware applications

# Hello World
hello: $(HELLO_SRC) | build_dir rom_lib
	$(CC) $(CFLAGS) -C $(ROM_CFG) $(HELLO_SRC) $(ROM_LIB) -o build/rom.bin

# Standard libraries

# TODO: don't really need to build the libs every time we build firmware
# Only rebuild when the crt0 files change

rom_lib:
	cp /usr/share/cc65/lib/supervision.lib rom.lib
	$(CA) -t none --cpu 65c02 -o crt0.o crt0.asm
	$(AR) a rom.lib crt0.o

# Cleanup

build_dir:
	@mkdir -p build

clean:
	@$(RM) -rf build src/*.o config/*.lib config/*.o build/*.map build/*.list
