#!/bin/sh

set -e

ARDUINO_CLI="/opt/local/arduino-ide_2.3.2_Linux_64bit/resources/app/lib/backend/resources/arduino-cli"
FQBN="MegaCoreX:megaavr:4809:clock=internal_20MHz,BOD=2v6,eeprom=keep,pinout=40pin_standard,resetpin=gpio,bootloader=no_bootloader"

# Cleanup
rm -rf *.o build 6502_vr_goggles/6502_rom.bin.h

echo "Building 6502 code"
make

echo "Converting 6502 binary to Arduino header file"
python3 generate_6502_arduino_header.py > 6502_vr_goggles/6502_rom_bin.h

echo "Building Arduino sketch"
$ARDUINO_CLI compile \
    --fqbn "$FQBN" 6502_vr_goggles/6502_vr_goggles.ino

echo "Upload Arduino sketch"
$ARDUINO_CLI upload \
    --programmer serialupdi_230400 \
    --port /dev/ttyUSB0 \
    --fqbn "$FQBN" \
    6502_vr_goggles/6502_vr_goggles.ino

echo "Done"

set +e
