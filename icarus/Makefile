# Makefile for Icarus Verilog simulation
# 02-11-2019 E. Brombaugh

# sources
SOURCES = 	tb_system.v ../src/system.v ../src/spram_16kx32.v \
			../src/acia.v ../src/acia_rx.v ../src/acia_tx.v \
			../src/wb_bus.v ../src/wb_master.v \
			../picorv32/picorv32.v 

# preparing the machine code
HEX = rom.hex

# top level
TOP = tb_system
			
# Executables
VLOG = iverilog
WAVE = gtkwave
TECH_LIB = /usr/local/share/yosys/ice40/cells_sim.v

# targets
all: $(TOP).vcd

$(HEX):
	$(MAKE) -C ../c/ main.hex
	cp ../c/main.hex ./$(HEX)
			
wave: $(TOP).vcd $(TOP).gtkw
	$(WAVE) $(TOP).gtkw
	
$(TOP).vcd: $(TOP)
	./$(TOP)

$(TOP): $(SOURCES) $(HEX)
	$(VLOG) -D icarus -l $(TECH_LIB) -o $(TOP) $(SOURCES)
	
clean:
	$(MAKE) -C ../c/ clean
	rm -rf a.out *.obj $(HEX) $(RPT) $(TOP) $(TOP).vcd
	
