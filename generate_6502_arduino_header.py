print("#define ROM_SIZE 0x1000")
print("const uint8_t ROM[ROM_SIZE] = {", end='')

with open("build/hello.bin", "rb") as f:
    while (byte := f.read(1)):
        # print("0x")
        print(hex(int.from_bytes(byte, "little")), end='')
        print(", ", end='')

print("};\n")
