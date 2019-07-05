	# Makefile for icestorm tools + yosys + nextpnr
# Modified from examples in icestorm distribution
# 01-16-18 E. Brombaugh

# src directory
VPATH = ../src:../picorv32

SRC =	up5k_riscv.v ../src/system.v ../src/spram_16kx32.v \
		../src/acia.v ../src/acia_rx.v ../src/acia_tx.v \
		../src/wb_bus.v ../src/wb_master.v \
		../picorv32/picorv32.v 

# preparing the machine code
FAKE_HEX =	rom.hex
REAL_HEX =  code.hex

# project stuff
PROJ = up5k_riscv
PIN_DEF = up5k_riscv.pcf
SDC = up5k_riscv.sdc
DEVICE = up5k

YOSYS = /usr/local/bin/yosys
NEXTPNR = nextpnr-ice40
NEXTPNR_ARGS = --pre-pack $(SDC) --placer heap --timing-allow-fail
ICEPACK = icepack
ICETIME = icetime
ICEPROG = iceprog
ICEBRAM = icebram
CDCPROG = cdc_prog
VERILATOR = verilator
TECH_LIB = /usr/local/share/yosys/ice40/cells_sim.v

all: $(PROJ).bin

$(FAKE_HEX):
	$(ICEBRAM) -g 32 2048 > $(FAKE_HEX)

%.json: $(SRC) $(FAKE_HEX)
	$(YOSYS) -p 'synth_ice40 -dsp -top $(PROJ) -json $@' $(SRC)

%.asc: %.json $(PIN_DEF) 
	$(NEXTPNR) $(NEXTPNR_ARGS) --$(DEVICE) --json $< --pcf $(PIN_DEF) --asc $@

$(REAL_HEX):
	$(MAKE) -C ../c/ main.hex
	cp ../c/main.hex ./$(REAL_HEX)
		
%.bin: %.asc $(REAL_HEX)
	$(ICEBRAM) $(FAKE_HEX) $(REAL_HEX) < $< > temp.asc
	$(ICEPACK) temp.asc $@

%.rpt: %.asc
	$(ICETIME) -d $(DEVICE) -mtr $@ $<

prog: $(PROJ).bin
	$(CDCPROG) -p /dev/ttyACM0 $<

recode:
	rm -f $(REAL_HEX) $(PROJ).bin
	$(MAKE) prog
    
flash: $(PROJ).bin
	$(CDCPROG) -w -p /dev/ttyACM0 $<

lint: $(SRC)
	$(VERILATOR) --lint-only -Wall --top-module $(PROJ) $(TECH_LIB) $(SRC)

clean:
	$(MAKE) -C ../c/ clean
	rm -f *.json *.asc *.rpt *.bin *.hex

.SECONDARY:
.PHONY: all prog clean
