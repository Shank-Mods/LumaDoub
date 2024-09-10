#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for
// information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA  4
#define I2C_SCL  5

typedef struct {
  uint8_t addr;      // device address to send the command to
  uint8_t reg;       // register to write to
  uint8_t val;       // value to write
  uint32_t delay_ms; // delay (in milliseconds) to wait before sending this command
} device_command_t;

device_command_t commands_freerun_480i_60Hz_YPbPr_out[] = {
  {0x20, 0x0F, 0x80, 10}, // Reset ADV7280A
  {0x2A, 0x17, 0x02, 0 }, // Reset Encoder
  {0x20, 0x0F, 0x00, 10}, // Exit Power Down Mode [ADV7280A writes begin]
  {0x20, 0x52, 0xCD, 0 }, // AFE IBIAS
  {0x20, 0x00, 0x07, 0 }, // ADI Required Write [INSEL set to unconnected input]
  {0x20, 0x0C, 0x37, 0 }, // Force Free run mode
  {0x20, 0x02, 0x54, 0 }, // Force standard to NTSC-M
  {0x20, 0x14, 0x11, 0 }, // Set Free-run pattern to 100% color bars
  {0x20, 0x80, 0x51, 0 }, // ADI Required Write
  {0x20, 0x81, 0x51, 0 }, // ADI Required Write
  {0x20, 0x82, 0x68, 0 }, // ADI Required Write
  {0x20, 0x17, 0x41, 0 }, // Enable SH1
  {0x20, 0x03, 0x0C, 0 }, // Enable Pixel & Sync output drivers
  {0x20, 0x04, 0x07, 0 }, // Power-up INTRQ, HS & VS pads
  {0x20, 0x13, 0x00, 0 }, // Enable ADV7280A for 28_63636MHz crystal
  {0x20, 0x1D, 0x40, 0 }, // Enable LLC output driver [ADV7280A writes finished]
  {0x2A, 0x00, 0x1C, 0 }, // Power up DACs and PLL [Encoder writes begin]
  {0x2A, 0x01, 0x00, 0 }, // Set Encoder to SD mode
  {0x2A, 0x80, 0x10, 0 }, // SSAF Luma filter enabled, NTSC mode
  {0x2A, 0x82, 0xC9, 0 }, // Step control on, pixel data valid, ped on, PbPr SSAF on, YPbPr out
  {0x2A, 0x87, 0x20, 0 }, // PAL/NTSC autodetect mode enabled
  {0x2A, 0x88, 0x00, 0 }, // 8 bit input enabled [Encoder Writes finished]
};

int i2c_write_to_device_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t val) {
  const uint8_t data[2] = {reg, val};
  return i2c_write_blocking(i2c, addr, data, 2, false);
};

int i2c_write_commands(i2c_inst_t *i2c, device_command_t *commands, int len) {
  int write_result;
  device_command_t *command;

  for (int i = 0; i < len; i++) {
    command = &commands[i];

    // delay if needed before sending command
    if (command->delay_ms > 0) {
      sleep_ms(command->delay_ms);
    }

    write_result =
        i2c_write_to_device_register(i2c, command->addr, command->reg, command->val);
    if (write_result < 0) {
      return write_result;
    }
  }

  return 0;
};

int main() {
  stdio_init_all();

  // I2C Initialisation. Using it at 100Khz.
  i2c_init(I2C_PORT, 100 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  // For more examples of I2C use see
  // https://github.com/raspberrypi/pico-examples/tree/master/i2c

  int ret;

  ret = i2c_write_commands(I2C_PORT, commands_freerun_480i_60Hz_YPbPr_out, 22);
  if (ret < 0)
    return ret;

  return 0;
}
