/*
 * i2c.c - i2c port driver
 * 06-23-20 E. Brombaugh
 */

#include <stdio.h>
#include "printf.h"
#include "clkcnt.h"
#include "i2c.h"
#include "up5k_riscv.h"

/* contro reg bits */
#define I2C_CCR_EN 0x80

#define I2C_CMD_STA 0x80
#define I2C_CMD_STO 0x40
#define I2C_CMD_RD 0x20
#define I2C_CMD_WR 0x10
#define I2C_CMD_ACK 0x08
#define I2C_CMD_CKSDIS 0x04

#define I2C_SR_TIP 0x80
#define I2C_SR_BUSY 0x40
#define I2C_SR_RARC 0x20
#define I2C_SR_SRW 0x10
#define I2C_SR_ARBL 0x08
#define I2C_SR_TRRDY 0x04
#define I2C_SR_TROE 0x02
#define I2C_SR_HGC 0x01

/* some common operation macros */
#define i2c_trrdy_wait(s) while(!(((s)->I2CSR)&I2C_SR_TRRDY))

/*
 * initialize i2c port
 */
void i2c_init(I2C_TypeDef *s)
{
#if 1
	
	s->I2CCR1 = I2C_CCR_EN | 12;	// enable I2C
	s->I2CBRMSB = 0;		// high 2 bits - resets I2C core
	s->I2CBRLSB = 60;		// low 8 bits - (24MHz/100kHz)/4 = 60
#else
	s->reserved0 = 0xff;	// 0
	s->reserved1 = 0xff;	// 1
	s->reserved2 = 0xff;	// 2
	s->I2CSADDR = 0xff;		// 3
	s->reserved4 = 0xff;	// 4
	s->reserved5 = 0xff;	// 5
	s->I2CIRQ = 0xff;		// 6
	s->I2CIRQEN = 0xff;		// 7
	s->I2CCR1 = 0xff;		// 8
	s->I2CCMDR = 0xff;		// 9
	s->I2CBRLSB = 0xff;		// A
	s->I2CBRMSB = 0xff;		// B
	s->I2CSR = 0xff;		// C
	s->I2CTXDR = 0xff;		// D
	s->I2CRXDR = 0xff;		// E
	s->I2CGCDR = 0xff;		// F

#endif
}

/*
 * i2c transmit bytes to addr
 */
int8_t i2c_tx(I2C_TypeDef *s, uint8_t addr, uint8_t *data, uint8_t sz)
{
	uint8_t err = 0;
	uint8_t stat;
	uint32_t timeout;
	
	/* diagnostic flag for I2C activity */
	gp_out |= 1;
	
	/* transmit address and no read bit */
	s->I2CTXDR = addr<<1;
	s->I2CCMDR = I2C_CMD_STA | I2C_CMD_WR | I2C_CMD_CKSDIS;

	/* loop over data */
	while(sz--)
	{
		/* wait for TRRDY */
		timeout = 3000;
		while(timeout--)
		{
			if(s->I2CSR & I2C_SR_TRRDY)
				break;
		}
		if(!timeout)
			err = 3;
		
		/* transmit data */
		s->I2CTXDR = *data++;
		s->I2CCMDR = I2C_CMD_WR | I2C_CMD_CKSDIS;
	}
	
	/* wait for TRRDY */
	timeout = 3000;
	while(timeout--)
	{
		if(s->I2CSR & I2C_SR_TRRDY)
			break;
	}
	if(!timeout)
		err = 3;
		
	/* send stop after last byte */
	s->I2CCMDR = I2C_CMD_STO | I2C_CMD_CKSDIS;
	
	/* check for error */
	stat = s->I2CSR;
	if(stat & I2C_SR_RARC)
	{
		if(stat & I2C_SR_TROE)
		{
			/* overrun + NACK */
			printf("i2c_tx: overrun error, status = %02X, resetting\n\r", s->I2CSR);
			err = 2;
			
			/* reset the core */
			s->I2CBRMSB = 0;
		}
		else
		{
			/* just a nack */
			err = 1;
		}
	}
	
	/* drop flag */
	gp_out &= ~1;
	
	return err;
}