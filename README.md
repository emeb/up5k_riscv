# up5k_riscv
There are many RISC V projects on iCE40. This one is mine.

## What is it?
A small RISC V system built on an iCE40 UP5k FPGA which includes the following:

* Claire Wolf's PicoRV32 CPU
* 8kB boot ROM in dedicated BRAM
* 64kB instruction/data RAM in SPRAM
* Dedicated hard IP core SPI interface to configuration flash
* Additional hard IP core SPI, currently used for an ILI9341 LCD
* Dedicated hard IP core I2C for testing
* 115k serial port
* 32-bit output port (for LEDs, LCD control, etc)
* GCC firmware build

## Prerequisites
To build this you will need the following FPGA tools

* Icestorm - ice40 FPGA tools
* Yosys - Synthesis
* Nextpnr - Place and Route (version newer than Mar 23 2019 is needed to support IP cores)

Info on these can be found at http://www.clifford.at/icestorm/

You will also need a RISC V GCC toolchain to build the startup ROM. I used:

* https://gnu-mcu-eclipse.github.io/blog/2018/10/19/riscv-none-gcc-v8-1-0-2-20181019-released/

## Building

	git clone https://github.com/emeb/up5k_riscv.git
	cd up5k_riscv
	git submodule update --init
	cd icestorm
	make

## Loading
I built this system on a custom up5k board and programmed it with a custom
USB->SPI board that I built so you will definitely need to tweak the programming
target of the Makefile in the icestorm directory to match your own hardware.

## Booting up
Connect a 115.2kbps terminal to the TX/RX pins of the FPGA and apply power.
You should see a message:

	up5k_riscv - starting up
	spi id: 0x00EF4016
	LCD initialized
	I2C0 Initialized
	xxxx...
	
If you have an LCD connected to the SPI1 port pins on the FPGA then it should
display several different screens to demonstrate the graphics. You can store
a raw rgb565-encoded image in 240x320 dimensions at flash location 0x200000
which will be BLITed to the screen. A helper script to properly format the
image is located in the "tools" directory.

A new addition is testing of the SB_I2C hard core. If you have an I2C device
on the bus at the expected address then you will see "." characters, otherwise
"x" will be printed.

## Thanks

Thanks to the developers of all the tools and cores used for this. In particular

* Claire Wolf <clifford@clifford.at> for the picorv32 and icestorm, etc.
* Sylvain Munaut for working examples and assistance.
* Dave Shah for knowing almost everything.
