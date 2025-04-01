#include "i2c.h"

int i2c_read_from_device_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t *val) {
  int ret = i2c_write_blocking_until(i2c, addr, &reg, 1, true, make_timeout_time_ms(100));
  if (ret < 0)
    return ret;

  ret = i2c_read_blocking_until(i2c, addr, val, 1, false, make_timeout_time_ms(100));
  if (!ret || ret == PICO_ERROR_TIMEOUT)
    return 0;
  else
    return ret;
};

int i2c_write_to_device_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t val) {
  const uint8_t data[2] = {reg, val};

  int ret = i2c_write_blocking_until(i2c, addr, data, 2, false, make_timeout_time_ms(100));
  if (!ret || ret == PICO_ERROR_TIMEOUT)
    return 0;
  else
    return ret;
};

int i2c_write_commands(i2c_inst_t *i2c, const device_command_t *commands, int len) {
  int write_result;
  const device_command_t *command;

  for (int i = 0; i < len; i++) {
    command = &commands[i]; // TODO: it reeeeeally seems like i shouldn't be able to do this

    // delay if needed before sending command
    if (command->delay_ms > 0) {
      sleep_ms(command->delay_ms);
    }

    write_result =
        i2c_write_to_device_register(i2c, command->addr, command->reg, command->val);
    if (write_result < 0) {
      return write_result;
    }

    // printf("\t\twrote command: 0x%02x 0x%02x 0x%02x\n", command->addr, command->reg,
    //  command->val);
  }

  return 0;
};