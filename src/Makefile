CC = cl65
CA = ca65
AR = ar65

CFLAGS = -O -t none --cpu 65c02 --static-locals

ROM_CFG = mem.cfg

ROM_LIB = rom.lib

MONITOR_SRC = monitor.c vr65c02.c

.PHONY: all clean

all: monitor

# ROM monitor application
monitor: $(MONITOR_SRC) | build_dir rom_lib
	$(CC) $(CFLAGS) -C $(ROM_CFG) $(MONITOR_SRC) $(ROM_LIB) -o build/rom.bin

# Standard libraries
# TODO: don't really need to build the libs every time we build firmware
# Only rebuild when the crt0 files change
rom_lib:
	cp /usr/share/cc65/lib/supervision.lib rom.lib
	$(CA) -t none --cpu 65c02 -o crt0.o crt0.asm
	$(AR) a rom.lib crt0.o

build_dir:
	@mkdir -p build

# Cleanup
clean:
	@$(RM) -rf build *.o *.lib 6502_vr_goggles/6502_rom_bin.h
