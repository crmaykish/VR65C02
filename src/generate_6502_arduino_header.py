print("#ifndef ROM_BIN_H")
print("#define ROM_BIN_H\r\n")

print('#include "memory_map.h"\r\n')

print("const uint8_t ROM[ROM_SIZE] = {", end='')

with open("build/rom.bin", "rb") as f:
    while (byte := f.read(1)):
        # print("0x")
        print(hex(int.from_bytes(byte, "little")), end='')
        print(", ", end='')

print("};\n")

print("#endif")
