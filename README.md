# rv

A risc-v cpu emulator

## Pre-requisites

In mac download the following package using brew:

```sh
brew install riscv64-elf-gcc riscv64-elf-binutils
```

## Testing

Now you can test using the following commands:

```
riscv64-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -Wl,-e,_start -Wl,-Ttext=0x0 -o tests/addi.elf tests/addi.s
riscv64-elf-objcopy -O binary tests/addi.elf tests/addi.bin
riscv64-elf-objdump -d tests/addi.elf
```

## Resources

- https://book.rvemu.app/hardware-components/01-cpu.html

