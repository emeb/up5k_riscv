CROSS = /opt/riscv-none-gcc/8.1.0-2-20181019-0952/bin/riscv-none-embed-
CC = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
ICEPROG = iceprog
HEXDUMP = hexdump
HEXDUMP_ARGS = -v -e '1/4 "%08x" "\n"'

#CFLAGS=-Wall -Os -march=rv32i -mabi=ilp32 -ffreestanding -nostartfiles -flto
CFLAGS=-Wall -Os -march=rv32i -mabi=ilp32 -ffreestanding -flto -nostartfiles -fomit-frame-pointer

HEADER = up5k_riscv.h acia.h spi.h flash.h clkcnt.h ili9341.h i2c.h printf.h

SOURCES = start.S main.c acia.c spi.c flash.c clkcnt.c ili9341.c i2c.c printf.c

main.elf: lnk-app.lds $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS)  -Wl,-Bstatic,-T,lnk-app.lds,--strip-debug -o $@ $(SOURCES)

disassemble: main.elf
	$(OBJDUMP) -d main.elf > main.dis

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.hex: %.bin
	$(HEXDUMP) $(HEXDUMP_ARGS) $< >$@

clean:
	rm -f *.bin *.hex *.elf *.dis
