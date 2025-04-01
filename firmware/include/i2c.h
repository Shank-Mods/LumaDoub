#ifndef I2C_H
#define I2C_H

#include "commands.h"
#include "defines.h"
#include "hardware/i2c.h"

int i2c_read_from_device_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t *val);
int i2c_write_to_device_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t val);
int i2c_write_commands(i2c_inst_t *i2c, const device_command_t *commands, int len);

#endif