// up5k_riscv.v - top level for riscv in up5k
// 07-01-19 E. Brombaugh

`default_nettype none

module up5k_riscv(
	// 16MHz clock osc
	input clk_16,
		
    // serial
    inout RX, TX,
	
	// SPI0 port hooked to cfg flash
	inout	spi0_mosi,
			spi0_miso,
			spi0_sclk,
			spi0_cs0,
	
	// SPI1 port on PMOD
	inout	spi1_mosi,
			spi1_miso,
			spi1_sclk,
			spi1_cs0,
	
	// GP Out for LCD
	output lcd_nrst, lcd_dc,
	
	// diagnostic
	output d1, d2,
		
	// LED - via drivers
	output RGB0, RGB1, RGB2
);
		
	// Fin=16, Fout=24 (16*(24/16))
	wire clk, pll_lock;
	SB_PLL40_PAD #(
		.DIVR(4'b0000),
		.DIVF(7'b0010111),
		.DIVQ(3'b100),
		.FILTER_RANGE(3'b001),
		.FEEDBACK_PATH("SIMPLE"),
		.DELAY_ADJUSTMENT_MODE_FEEDBACK("FIXED"),
		.FDA_FEEDBACK(4'b0000),
		.DELAY_ADJUSTMENT_MODE_RELATIVE("FIXED"),
		.FDA_RELATIVE(4'b0000),
		.SHIFTREG_DIV_MODE(2'b00),
		.PLLOUT_SELECT("GENCLK"),
		.ENABLE_ICEGATE(1'b0)
	)
	pll_inst (
		.PACKAGEPIN(clk_16),
		.PLLOUTCORE(clk),
		.PLLOUTGLOBAL(),
		.EXTFEEDBACK(),
		.DYNAMICDELAY(8'h00),
		.RESETB(1'b1),
		.BYPASS(1'b0),
		.LATCHINPUTVALUE(),
		.LOCK(pll_lock),
		.SDI(),
		.SDO(),
		.SCLK()
	);
	
	// reset generator waits > 10us afer PLL lock
	reg [7:0] reset_cnt;
	reg reset;    
	always @(posedge clk)
	begin
		if(!pll_lock)
		begin
			reset_cnt <= 8'h00;
			reset <= 1'b1;
		end
		else
		begin
			if(reset_cnt != 8'hff)
			begin
				reset_cnt <= reset_cnt + 8'h01;
				reset <= 1'b1;
			end
			else
				reset <= 1'b0;
		end
	end
	
	// system core
	wire [3:0] tst;
	wire [31:0] gpio_o;
	wire raw_rx, raw_tx;
	system uut(
		.clk24(clk),
		.reset(reset),
		
		.RX(raw_rx),
		.TX(raw_tx),
		
		.spi0_mosi(spi0_mosi),
		.spi0_miso(spi0_miso),
		.spi0_sclk(spi0_sclk),
		.spi0_cs0(spi0_cs0),
	
		.spi1_mosi(spi1_mosi),
		.spi1_miso(spi1_miso),
		.spi1_sclk(spi1_sclk),
		.spi1_cs0(spi1_cs0),
	
		.gp_out(gpio_o)
	);
	
	// Serial I/O w/ pullup on RX
	SB_IO #(
		.PIN_TYPE(6'b101001),
		.PULLUP(1'b1),
		.NEG_TRIGGER(1'b0),
		.IO_STANDARD("SB_LVCMOS")
	) urx_io (
		.PACKAGE_PIN(RX),
		.LATCH_INPUT_VALUE(1'b0),
		.CLOCK_ENABLE(1'b0),
		.INPUT_CLK(1'b0),
		.OUTPUT_CLK(1'b0),
		.OUTPUT_ENABLE(1'b0),
		.D_OUT_0(1'b0),
		.D_OUT_1(1'b0),
		.D_IN_0(raw_rx),
		.D_IN_1()
	);
	SB_IO #(
		.PIN_TYPE(6'b101001),
		.PULLUP(1'b0),
		.NEG_TRIGGER(1'b0),
		.IO_STANDARD("SB_LVCMOS")
	) utx_io (
		.PACKAGE_PIN(TX),
		.LATCH_INPUT_VALUE(1'b0),
		.CLOCK_ENABLE(1'b0),
		.INPUT_CLK(1'b0),
		.OUTPUT_CLK(1'b0),
		.OUTPUT_ENABLE(1'b1),
		.D_OUT_0(raw_tx),
		.D_OUT_1(1'b0),
		.D_IN_0(),
		.D_IN_1()
	);
	
	assign d1 = gpio_o[0];
	assign d2 = raw_tx;
	
	// RGB LED Driver IP core
	SB_RGBA_DRV #(
		.CURRENT_MODE("0b1"),
		.RGB0_CURRENT("0b000001"),
		.RGB1_CURRENT("0b000001"),
		.RGB2_CURRENT("0b000011")
	) RGBA_DRIVER (
		.CURREN(1'b1),
		.RGBLEDEN(1'b1),
		.RGB0PWM(gpio_o[17]),
		.RGB1PWM(gpio_o[18]),
		.RGB2PWM(gpio_o[19]),
		.RGB0(RGB0),
		.RGB1(RGB1),
		.RGB2(RGB2)
	);
	
	// LCD control lines
	assign lcd_nrst = gpio_o[31];
	assign lcd_dc = gpio_o[30];
endmodule
