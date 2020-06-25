/*
 * i2c.h - I2C IP core driver
 * 06-23-20 E. Brombaugh
 */

#ifndef __i2c__
#define __i2c__

#include "up5k_riscv.h"

/* i2c functions */
void i2c_init(I2C_TypeDef *s);
int8_t i2c_tx(I2C_TypeDef *s, uint8_t addr, uint8_t *data, uint8_t sz);

#endif

