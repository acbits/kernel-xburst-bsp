#ifndef __I2C_GPIO_H
#define __I2C_GPIO_H

struct i2c_gpio {
	unsigned int scl;
	unsigned int sda;
};

void i2c_init(struct i2c_gpio *i2c);

int  i2c_write(struct i2c_gpio *i2c,unsigned char chip,
		unsigned int addr, int alen, unsigned char *buffer, int len);

int  i2c_read(struct i2c_gpio *i2c,unsigned char chip,
		unsigned int addr, int alen, unsigned char *buffer, int len);

int i2c_probe(struct i2c_gpio *i2c, unsigned char addr);

#endif /* __I2C_GPIO_H */
