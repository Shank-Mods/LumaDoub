#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for
// information on GPIO assignments
#define I2C_PORT              i2c0
#define I2C_SDA               4
#define I2C_SCL               5
#define DEVICE_RESET_PIN_7280 2 // active low
#define DEVICE_RESET_PIN_7391 3 // active low

#define SWITCH_CVBS_IN      6  // active low
#define SWITCH_YC_IN        7  // active low
#define SWITCH_YPBPR_OUT    8  // active low
#define SWITCH_CVBS_OUT     9  // active low
#define SWITCH_LINE_DOUBLE  10 // active low
#define SWITCH_VASELINE     11 // active low
#define SWITCH_NOTCH_FILTER 12 // active low
#define SWITCH_FORCE_240P   13 // active low

// the following defines select one register whose bits are controlled by the on-board register bits switches.
// device addresses to choose from:
//    0x20: 7280's main (user sub) map
//    0x42: 7280's VPP map
//    0x2A: 7391
#define DEVICE_SWITCH_BITS     0x42 // device address, as given above
#define REGISTER_SWITCH_BITS   0x00 // register sub-address in that device
#define REGISTER_DEFAULT_VALUE 0x00 // default value for that register
#define SWITCH_B7              14   // active low
#define SWITCH_B6              15   // active low
#define SWITCH_B5              16   // active low
#define SWITCH_B4              17   // active low
#define SWITCH_B3              18   // active low
#define SWITCH_B2              19   // active low
#define SWITCH_B1              20   // active low
#define SWITCH_B0              21   // active low

#define LOOP_RATE_MS 50

typedef struct {
  uint8_t addr;      // device address to send the command to
  uint8_t reg;       // register to write to
  uint8_t val;       // value to write
  uint32_t delay_ms; // delay (in milliseconds) to wait before sending this command
} device_command_t;

device_command_t commands_freerun_480i_60Hz_YPbPr_out[] = {
  // {0x20, 0x0F, 0x80, 10}, // Reset ADV7280A (TODO: enabling this command kills I2C, why?)
  {0x2A, 0x17, 0x02, 0 }, // Reset Encoder
  {0x20, 0x0F, 0x00, 10}, // Exit Power Down Mode [ADV7280A writes begin]
  // {0x20, 0x52, 0xCD, 0 }, // AFE IBIAS
  // {0x20, 0x54, 0xC0, 0 }, // AFE IBIAS             ***Reserved bits in the "interrupt clear" diagnostic pin
  // {0x20, 0x00, 0x0C, 0 }, // INSEL = YPbPr, Y=Ain1, Pb=Ain2, Pr=Ain3 (COMPONENT IN)
  // {0x20, 0x00, 0x00, 0 }, // INSEL = YPbPr, Y=Ain1, Pb=Ain2, Pr=Ain3 (COMPOSITE IN)
  // {0x20, 0x0C, 0x37, 0 }, // Force Free run mode
  {0x20, 0x02, 0x54, 0 }, // Force standard to NTSC-M
  {0x20, 0x51, 0xC4, 0 }, // Evaluate horizontal lock using fewer lines
  // {0x20, 0x14, 0x11, 0 }, // Set Free-run pattern to 100% color bars
  {0x20, 0x80, 0x51, 0 }, // ADI Required Write       ***Affects reserved bits for ACE. Consider experimenting with disabling*** ***CONSIDER SETTING TO 0X00. THIS IS CONTRAST BOOST, ACE)***
  {0x20, 0x81, 0x51, 0 }, // ADI Required Write       ***Undocumented, within ACE section. Consider experimenting with disabling***
  {0x20, 0x82, 0x68, 0 }, // ADI Required Write       ***Affects Pal. Says "Not Applicable for NTSC"***
  {0x20, 0x17, 0x41, 0 }, // Enable SH1               ***This sets a non-standard "C filter" (SH1), may only affect composite***
  {0x20, 0x03, 0x0C, 0 }, // Enable Pixel & Sync output drivers
  {0x20, 0x04, 0x07, 0 }, // Power-up INTRQ, HS & VS pads
  {0x20, 0x13, 0x00, 0 }, // Enable ADV7280A for 28_63636MHz crystal
  {0x20, 0x1D, 0x40, 0 }, // Enable LLC output driver
  {0x20, 0xFD, 0x84, 0 }, // Set VPP Map [ADV7280A writes finished]
  // {0x2A, 0x00, 0x1C, 0 }, // Power up DACs and PLL [Encoder writes begin]
  // {0x2A, 0x01, 0x00, 0 }, // Set Encoder to SD mode
  // {0x2A, 0x02, 0x10, 0 }, // RGB output enabled; RGB output sync enabled
  // {0x2A, 0x80, 0x10, 0 }, // SSAF Luma filter enabled, NTSC mode
  // {0x2A, 0x82, 0xC9, 0 }, // Step control on, pixel data valid, ped on, PbPr SSAF on
  // {0x2A, 0x87, 0x20, 0 }, // PAL/NTSC autodetect mode enabled
  // {0x2A, 0x88, 0x00, 0 }, // 8 bit input enabled [Encoder Writes finished]
};

device_command_t commands_cvbs_input[] = {
  {0x20, 0x0C, 0x34, 0}, // Force disable free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D***
  {0x20, 0x52, 0xCD, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x03, 0}, // CVBS in on Ain4                   ***Bits 7, 6, and 5 are blank and undocumented. Investigate?***
};

device_command_t commands_yc_input[] = {
  {0x20, 0x0C, 0x34, 0}, // Force disable free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D [default settings]***
  {0x20, 0x53, 0xCE, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x09, 0}, // INSEL = YC, Y - Ain3, C - Ain4
};

device_command_t commands_ypbpr_input[] = {
  {0x20, 0x0C, 0x34, 0}, // Force disable free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D [default settings]***
  {0x20, 0x54, 0xC0, 0}, // AFE IBIAS                         ***UNDOCUMENTED. Reserved bit in Interrupt clear diagnostic register? This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x0C, 0}, // INSEL = YPbPr, Y=Ain1, Pb=Ain2, Pr=Ain3
};

device_command_t commands_test_pattern_input[] = {
  //  {0x20, 0x00, 0x07, 0}, // ADI Required Write [INSEL set to unconnected input]
  //  {0x20, 0x0C, 0x37, 0}, // Force Free run mode
  //  {0x20, 0x14, 0x12, 0}, // Set Free-run pattern to luma ramp

};

device_command_t commands_rgb_output[] = {
  {0x2A, 0x02, 0x10, 0}, // RGB output enabled; RGB output sync enabled
  {0x2A, 0x82, 0xC9, 0}, // pixel data valid; RGB out; PbPr SSAF on; step control on; ped on
};

device_command_t commands_ypbpr_output[] = {
  {0x2A, 0x02, 0x20, 0}, // RGB output disabled (reset register to default value)
  {0x2A, 0x82, 0xC9, 0}, // pixel data valid; YPbPr out; PbPr SSAF on; step control on; ped on
};

device_command_t commands_cvbs_output[] = {
  {0x2A, 0x02, 0x20, 0}, // RGB output disabled (reset register to default value)
  {0x2A, 0x82, 0xCB, 0}, // pixel data valid; CVBS/Y-C out; PbPr SSAF on; step control on; ped on
};

device_command_t commands_line_double[] = {
  {0x42, 0xA3, 0x00, 0}, // ADI Required Write [ADV7280A VPP writes begin]
  {0x42, 0x5B, 0x00, 0}, // Enable Advanced Timing Mode
  {0x42, 0x55, 0x80, 0}, // Enable the Deinterlacer for I2P [ADV7280A VPP writes finished]

  {0x2A, 0x00, 0x9C, 0}, // Power up DACs and PLL [Encoder writes begin]
  {0x2A, 0x01, 0x70, 0}, // ED at 54MHz input

  {0x2A, 0x30, 0x04, 0}, // 525p at 59.94 Hz with Embedded Timing
  {0x2A, 0x31, 0x01, 0}, // ED Pixel Data Valid [Encoder Writes finished]

  {0x2A, 0x80, 0x00, 0}, // SSAF Luma filter enabled, NTSC mode (TODO: same as reset value, so not needed here?)
  {0x2A, 0x87, 0x00, 0}, // PAL/NTSC autodetect mode disabled (Reset value for SD register)
};

device_command_t commands_line_undouble[] = {
  {0x42, 0xA3, 0x00, 0}, // ADI Required Write [ADV7280A VPP writes begin]
  {0x42, 0x5B, 0x80, 0}, // Disable Advanced Timing Mode
  {0x42, 0x55, 0x00, 0}, // Disable I2P [ADV7280A VPP writes finished]

  {0x2A, 0x00, 0x1C, 0}, // Power up DACs and PLL [Encoder writes begin]
  {0x2A, 0x01, 0x00, 0}, // Set Encoder to SD mode

  {0x2A, 0x30, 0x00, 0}, // Reset value for ED register
  {0x2A, 0x31, 0x00, 0}, // Reset value for ED register

  {0x2A, 0x80, 0x00, 0}, // SSAF Luma filter enabled, NTSC mode (TODO: same as reset value, so not needed here?)
  {0x2A, 0x87, 0x20, 0}, // PAL/NTSC autodetect mode enabled
};

device_command_t commands_vaseline_on[] = {
  // {0x20, 0xF3, 0x1F, 0}, // Enable all antialiasing filters
  // {0x20, 0x17, 0b00000001, 0}, // Enable Chroma Filter
  // {0x42, 0x17, 0b00011000, 0}, // Enable Secret Deinterlacer
  {0x20, 0xF3, 0b00011111, 0}, // Enable  AA filters
  {0x20, 0x0E, 0x40,       0}, //enter user sub map 2
  {0x20, 0x80, 0x40,       0}, //Disable ACE
  {0x20, 0x0E, 0x00,       0}, //Leave user sub map 2, Re-enter User Sub map 1
};

device_command_t commands_vaseline_off[] = {
  // {0x20, 0xF3, 0x10, 0}, // Disable all antialiasing filters
  // {0x20, 0x17, 0b00100001, 0}, // Disables Chroma FIlter
  // {0x42, 0x17, 0b00000000, 0}, // Enable Standard Deinterlacer
  {0x20, 0xF3, 0b00010000, 0}, // Disable AA filters
  {0x20, 0x0E, 0x40,       0}, //enter user sub map 2
  {0x20, 0x80, 0x00,       0}, //Disable ACE
  {0x20, 0x0E, 0x00,       0}, //Leave user sub map 2, Re-enter User Sub map 1
};

device_command_t commands_notch_filter_on[] = {
  {0x20, 0x38, 0b10000100, 0}, //Force notch filter
};

device_command_t commands_notch_filter_off[] = {
  {0x20, 0x38, 0b10000000, 0}, //Use automatic filter
};

device_command_t commands_force_240p[] = {
  {0x2A, 0x88, 0x02, 0}, // 8 bit input enabled, SD noninterlaced mode on
};

device_command_t commands_unforce_240p[] = {
  {0x2A, 0x88, 0x00, 0}, // 8 bit input enabled, SD noninterlaced mode off
};
//*** */
device_command_t commands_autocvbs1_input[] = {
  {0x20, 0x0C, 0x36, 0}, // Don't force free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D***
  {0x20, 0x52, 0xCD, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x00, 0}, // CVBS in on Ain1                   ***Bits 7, 6, and 5 are blank and undocumented. Investigate?***
};

device_command_t commands_autocvbs2_input[] = {
  {0x20, 0x0C, 0x36, 0}, // Don't force free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D***
  {0x20, 0x52, 0xCD, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x01, 0}, // CVBS in on Ain1                   ***Bits 7, 6, and 5 are blank and undocumented. Investigate?***
};
device_command_t commands_autocvbs3_input[] = {
  {0x20, 0x0C, 0x36, 0}, // Don't force free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D***
  {0x20, 0x52, 0xCD, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x02, 0}, // CVBS in on Ain1                   ***Bits 7, 6, and 5 are blank and undocumented. Investigate?***
};
//*** */

int i2c_read_from_device_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t *val) {
  int ret = i2c_write_blocking_until(i2c, addr, &reg, 1, true, make_timeout_time_ms(100));
  if (!ret || ret == PICO_ERROR_TIMEOUT)
    return 0;

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

  gpio_init(DEVICE_RESET_PIN_7280);
  gpio_set_dir(DEVICE_RESET_PIN_7280, GPIO_OUT);

  gpio_put(DEVICE_RESET_PIN_7280, 0);
  sleep_ms(750);
  gpio_put(DEVICE_RESET_PIN_7280, 1);
  sleep_ms(10);

  gpio_init(DEVICE_RESET_PIN_7391);
  gpio_set_dir(DEVICE_RESET_PIN_7391, GPIO_OUT);

  gpio_put(DEVICE_RESET_PIN_7391, 0);
  sleep_ms(750);
  gpio_put(DEVICE_RESET_PIN_7391, 1);
  sleep_ms(10);

  gpio_init(SWITCH_CVBS_IN);
  gpio_set_dir(SWITCH_CVBS_IN, GPIO_IN);
  gpio_pull_up(SWITCH_CVBS_IN);

  gpio_init(SWITCH_YC_IN);
  gpio_set_dir(SWITCH_YC_IN, GPIO_IN);
  gpio_pull_up(SWITCH_YC_IN);

  gpio_init(SWITCH_YPBPR_OUT);
  gpio_set_dir(SWITCH_YPBPR_OUT, GPIO_IN);
  gpio_pull_up(SWITCH_YPBPR_OUT);

  gpio_init(SWITCH_CVBS_OUT);
  gpio_set_dir(SWITCH_CVBS_OUT, GPIO_IN);
  gpio_pull_up(SWITCH_CVBS_OUT);

  gpio_init(SWITCH_LINE_DOUBLE);
  gpio_set_dir(SWITCH_LINE_DOUBLE, GPIO_IN);
  gpio_pull_up(SWITCH_LINE_DOUBLE);

  gpio_init(SWITCH_VASELINE);
  gpio_set_dir(SWITCH_VASELINE, GPIO_IN);
  gpio_pull_up(SWITCH_VASELINE);

  gpio_init(SWITCH_NOTCH_FILTER);
  gpio_set_dir(SWITCH_NOTCH_FILTER, GPIO_IN);
  gpio_pull_up(SWITCH_NOTCH_FILTER);

  gpio_init(SWITCH_FORCE_240P);
  gpio_set_dir(SWITCH_FORCE_240P, GPIO_IN);
  gpio_pull_up(SWITCH_FORCE_240P);

  gpio_init(SWITCH_B0);
  gpio_set_dir(SWITCH_B0, GPIO_IN);
  gpio_pull_up(SWITCH_B0);

  gpio_init(SWITCH_B1);
  gpio_set_dir(SWITCH_B1, GPIO_IN);
  gpio_pull_up(SWITCH_B1);

  gpio_init(SWITCH_B2);
  gpio_set_dir(SWITCH_B2, GPIO_IN);
  gpio_pull_up(SWITCH_B2);

  gpio_init(SWITCH_B3);
  gpio_set_dir(SWITCH_B3, GPIO_IN);
  gpio_pull_up(SWITCH_B3);

  gpio_init(SWITCH_B4);
  gpio_set_dir(SWITCH_B4, GPIO_IN);
  gpio_pull_up(SWITCH_B4);

  gpio_init(SWITCH_B5);
  gpio_set_dir(SWITCH_B5, GPIO_IN);
  gpio_pull_up(SWITCH_B5);

  gpio_init(SWITCH_B6);
  gpio_set_dir(SWITCH_B6, GPIO_IN);
  gpio_pull_up(SWITCH_B6);

  gpio_init(SWITCH_B7);
  gpio_set_dir(SWITCH_B7, GPIO_IN);
  gpio_pull_up(SWITCH_B7);

  int ret = i2c_write_commands(I2C_PORT, commands_freerun_480i_60Hz_YPbPr_out, 13);
  if (ret < 0)
    return ret;

  bool cvbs_in, yc_in, ypbpr_out, cvbs_out, autocycle;
  bool line_double, vaseline_on, notch_filter, force_240p;
  bool bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0;
  bool is_interlaced, in_lock, fsc_lock, standard_line_length, standard_field_length, hlock;
  bool valid_ypbpr, valid_yc, valid_cvbs;

  int current_input, next_input; // 1 is YPbPr, 2 is YC, 3 is CVBS
  uint8_t read_value2, read_value3, read_value4;

  ret = i2c_write_commands(I2C_PORT, commands_ypbpr_input, 4);
  if (ret < 0)
    return ret;

  current_input = 1; // 1 is YPbPr, 2 is YC, 3 is CVBS

  while (true) {
    sleep_ms(LOOP_RATE_MS);

    cvbs_in = !gpio_get(SWITCH_CVBS_IN);           // active low
    yc_in = !gpio_get(SWITCH_YC_IN);               // active low
    ypbpr_out = !gpio_get(SWITCH_YPBPR_OUT);       // active low
    cvbs_out = !gpio_get(SWITCH_CVBS_OUT);         // active low
    line_double = !gpio_get(SWITCH_LINE_DOUBLE);   // active low
    vaseline_on = !gpio_get(SWITCH_VASELINE);      // active low
    notch_filter = !gpio_get(SWITCH_NOTCH_FILTER); // active low
    force_240p = !gpio_get(SWITCH_FORCE_240P);     // active low

    bit0 = !gpio_get(SWITCH_B0); // active low
    bit1 = !gpio_get(SWITCH_B1); // active low
    bit2 = !gpio_get(SWITCH_B2); // active low
    bit3 = !gpio_get(SWITCH_B3); // active low
    bit4 = !gpio_get(SWITCH_B4); // active low
    bit5 = !gpio_get(SWITCH_B5); // active low
    bit6 = !gpio_get(SWITCH_B6); // active low
    bit7 = !gpio_get(SWITCH_B7); // active low

    // input (or autocycle)
    if (!cvbs_in && !yc_in) {
      ret = i2c_write_commands(I2C_PORT, commands_ypbpr_input, 4);
      if (ret < 0)
        return ret;

      autocycle = 0;
      current_input = 1; // 1 is YPbPr, 2 is YC, 3 is CVBS

    } else if (cvbs_in && !yc_in) {
      ret = i2c_write_commands(I2C_PORT, commands_cvbs_input, 4);
      if (ret < 0)
        return ret;

      autocycle = 0;
      current_input = 3; // 1 is YPbPr, 2 is YC, 3 is CVBS

    } else if (!cvbs_in && yc_in) {
      ret = i2c_write_commands(I2C_PORT, commands_yc_input, 4);
      if (ret < 0)
        return ret;

      autocycle = 0;
      current_input = 2; // 1 is YPbPr, 2 is YC, 3 is CVBS

    } else {
      autocycle = 1;
    }

    // output
    if (ypbpr_out) {
      ret = i2c_write_commands(I2C_PORT, commands_ypbpr_output, 2);
    } else if (cvbs_out) {
      ret = i2c_write_commands(I2C_PORT, commands_cvbs_output, 2);
    } else {
      ret = i2c_write_commands(I2C_PORT, commands_rgb_output, 2);
    }

    if (ret < 0)
      return ret;

    // force 240p
    ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x13, &read_value4);
    if (ret < 0)
      return ret;

    is_interlaced = read_value4 & 0b01000000;

    if (force_240p || !is_interlaced) {
      ret = i2c_write_commands(I2C_PORT, commands_force_240p, 1);
    } else {
      ret = i2c_write_commands(I2C_PORT, commands_unforce_240p, 1);
    }

    if (ret < 0)
      return ret;

    // line double
    if (line_double) {
      ret = i2c_write_commands(I2C_PORT, commands_line_double, 9);
    } else {
      ret = i2c_write_commands(I2C_PORT, commands_line_undouble, 9);
    }

    if (ret < 0)
      return ret;

    // vaseline filter
    if (vaseline_on) {
      ret = i2c_write_commands(I2C_PORT, commands_vaseline_on, 4);
    } else {
      ret = i2c_write_commands(I2C_PORT, commands_vaseline_off, 4);
    }

    if (ret < 0)
      return ret;

    // notch filter
    if (notch_filter) {
      ret = i2c_write_commands(I2C_PORT, commands_notch_filter_on, 1);
    } else {
      ret = i2c_write_commands(I2C_PORT, commands_notch_filter_off, 1);
    }

    if (ret < 0)
      return ret;

    // custom register
    // if (notch_filter) {
    //    uint8_t register_bits = ((bit7 << 7) | (bit6 << 6) | (bit5 << 5) | (bit4 << 4) | (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | (bit0 << 0));
    //    ret = i2c_write_to_device_register(I2C_PORT, DEVICE_SWITCH_BITS, REGISTER_SWITCH_BITS, register_bits);
    // } else {
    //    ret = i2c_write_to_device_register(I2C_PORT, DEVICE_SWITCH_BITS, REGISTER_SWITCH_BITS, REGISTER_DEFAULT_VALUE);
    // }

    // if (ret < 0)
    //   return ret;

    if (!autocycle)
      continue;

    ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x10, &read_value2);
    if (ret < 0)
      return ret;

    ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x12, &read_value3);
    if (ret < 0)
      return ret;

    ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x13, &read_value4);
    if (ret < 0)
      return ret;

    in_lock = read_value2 & 0b00000001;
    fsc_lock = read_value2 & 0b00000100;
    standard_line_length = !(read_value3 & 0b00010000);
    standard_field_length = read_value4 & 0b00100000;
    hlock = read_value4 & 0b00000001;

    printf("input: ");
    if (current_input == 1) {
      printf("ypbpr ");
    } else if (current_input == 2) {
      printf("yc    ");
    } else if (current_input == 3) {
      printf("cvbs  ");
    }

    printf(" |  hlock = %d/%d, fsc lock = %d  |  0x%02x 0x%02x 0x%02x\n", in_lock, hlock, fsc_lock, read_value2, read_value3, read_value4);

    // if we have hlock, and if we're not in the case where input is YC and we have an invalid line length, no need to keep cycling
    valid_ypbpr = (current_input == 1 && hlock);
    valid_yc = (current_input == 2 && hlock);
    valid_cvbs = (current_input == 3 && hlock);
    if (valid_ypbpr || valid_yc || valid_cvbs) {
      next_input = 1;
      continue;
    }

    if (next_input == 1) {
      // try YPbPr
      ret = i2c_write_commands(I2C_PORT, commands_ypbpr_input, 4);
      if (ret < 0)
        return ret;

      current_input = 1; // 1 is YPbPr, 2 is YC, 3 is CVBS
      next_input = 3;

    } else if (next_input == 3) {
      // try CVBS
      ret = i2c_write_commands(I2C_PORT, commands_cvbs_input, 4);
      if (ret < 0)
        return ret;

      current_input = 3; // 1 is YPbPr, 2 is YC, 3 is CVBS
      next_input = 2;

    } else if (next_input == 2) {
      // try YC
      ret = i2c_write_commands(I2C_PORT, commands_yc_input, 4);
      if (ret < 0)
        return ret;

      current_input = 2; // 1 is YPbPr, 2 is YC, 3 is CVBS
      next_input = 1;
    }
  }

  return 0;
}