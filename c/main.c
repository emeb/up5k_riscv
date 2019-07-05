/*
 * main.c - top level of picorv32 firmware
 * 06-30-19 E. Brombaugh
 */

#include <stdio.h>
#include "up5k_riscv.h"
#include "acia.h"
#include "spi.h"
#include "flash.h"
#include "clkcnt.h"
#include "ili9341.h"

/*
 * convert a u32 to string of n hex digits
 */
void u32tostr(char *dst, uint32_t val, uint8_t n)
{
	uint8_t c;
	
	/* loop over 8 nybbles, left to right */
	while(n)
	{
		/* get high nybble as ascii hex */
		c = (val>>(4*(n-1)))&0xf;
		c = (c>9) ? c + 7 : c;
		*dst++ = c + 0x30;
		n--;
	}
	
	/* add trailing null */
	*dst = 0;
}

/*
 * main... duh
 */
void main()
{
	uint32_t cnt, spi_id, i, j;
	//int c;
	char buffer[32];
	
	acia_puts("\n\r\n\rup5k_riscv - starting up\n\r");
	
	/* test both SPI ports */
	spi_init(SPI0);
	spi_init(SPI1);
	
	/* get spi flash id */
	flash_init(SPI0);	// wake up the flash chip
	spi_id = flash_id(SPI0);
	u32tostr(buffer, spi_id, 8);
	acia_puts("spi id: 0x");
	acia_puts(buffer);
	acia_puts("\n\r");
	
#if 0
	/* read some data */
	{
		uint8_t read[256];
		flash_read(SPI0, read, 0, 256);
		for(i=0;i<256;i+=8)
		{
			u32tostr(buffer, i, 2);
			acia_puts(buffer);
			acia_puts(": ");
			for(j=0;j<8;j++)
			{
				u32tostr(buffer, read[i+j], 2);
				acia_puts(buffer);
				acia_puts(" ");
			}
			acia_puts("\n\r");
		}
	}
#endif

	/* Test LCD */
	ili9341_init(SPI1);
	
#if 1
	/* color fill + text fonts */
	ili9341_fillRect(20, 20, 200, 280, ILI9341_MAGENTA);
	ili9341_drawstr(120-44, (160-12*8), "Hello World", ILI9341_WHITE, ILI9341_MAGENTA);
	
	/* test font */
	for(i=0;i<256;i+=16)
		for(j=0;j<16;j++)
			ili9341_drawchar((120-8*8)+(j*8), (160-8*8)+(i/2), i+j,
				ILI9341_GREEN, ILI9341_BLACK);
	
	clkcnt_delayms(1000);
#endif
	
#if 0
	/* test colored lines */
	{
		uint8_t rgb[3], hsv[3];
		uint16_t color;
		ili9341_fillScreen(ILI9341_BLACK);
		hsv[1] = 255;
		hsv[2] = 255;
		j=256;
		while(j--)
		{	
			for(i=0;i<320;i++)
			{
				hsv[0] = (i+j);
				
				ili9341_hsv2rgb(rgb, hsv);
				color = ili9342_Color565(rgb[0],rgb[1],rgb[2]);
		#if 0
				ili9341_drawLine(i, 0, ILI9341_TFTWIDTH-1, i, color);
				ili9341_drawLine(ILI9341_TFTWIDTH-1, i, ILI9341_TFTWIDTH-1-i, ILI9341_TFTWIDTH-1, color);
				ili9341_drawLine(ILI9341_TFTWIDTH-1-i, ILI9341_TFTWIDTH-1, 0, ILI9341_TFTWIDTH-1-i, color);
				ili9341_drawLine(0, ILI9341_TFTWIDTH-1-i, i, 0, color);
		#else
				ili9341_drawFastHLine(0, i, 240, color);
		#endif
			}
		}
	}
	clkcnt_delayms(1000);
#endif

#if 1
	/* test image blit from flash */
	{
		uint16_t blit[ILI9341_TFTWIDTH*4];
		uint32_t blitaddr, blitsz;
		blitaddr = 0x200000;
		blitsz = ILI9341_TFTWIDTH*4*sizeof(uint16_t);
		for(i=0;i<ILI9341_TFTHEIGHT;i+=4)
		{
			flash_read(SPI0, (uint8_t *)blit, blitaddr, blitsz);
			ili9341_blit(0, i, ILI9341_TFTWIDTH, 4, blit);
			blitaddr += blitsz;
		}
	}
#endif

	cnt = 0;
	while(1)
	{
		gp_out = (gp_out&~(7<<17))|((cnt&7)<<17);
		cnt++;
		acia_putc('.');
		
#if 0
		/* simple echo */
		if((c=acia_getc()) != EOF)
			acia_putc(c);
#endif		
		clkcnt_delayms(1000);
	}
}
