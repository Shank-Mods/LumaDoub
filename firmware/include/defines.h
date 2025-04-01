#ifndef DEFINES_H
#define DEFINES_H

#include "pico/stdlib.h"

#define HSyncIn 26 // GPIO pin for the Hsync INPUT  INTERRUPT TEST
// #define VSyncIn     27 // GPIO pin for the output bit    INTERRUPT TEST
// #define THS_Disable 28 // GPIO pin for THS7374 Disable pin INTERRUPT TEST

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
#define DEVICE_SWITCH_BITS     0x20 // device address, as given above
#define REGISTER_SWITCH_BITS   0xFF // register sub-address in that device
#define REGISTER_DEFAULT_VALUE 0x1A // default value for that register
#define SWITCH_B7              14   // active low
#define SWITCH_B6              15   // active low
#define SWITCH_B5              16   // active low
#define SWITCH_B4              17   // active low
#define SWITCH_B3              18   // active low
#define SWITCH_B2              19   // active low
#define SWITCH_B1              20   // active low
#define SWITCH_B0              21   // active low

#define SWITCH_RGB_SYNC           SWITCH_B6
#define SWITCH_PBPR_SYNC          SWITCH_B5
#define SWITCH_BETA_INPUT_VOLTAGE SWITCH_B4
#define SWITCH_TEST_PATTERN_IN    SWITCH_B0
#define SWITCH_PEDESTAL_OUTPUT    SWITCH_B7

#define LOOP_RATE_MS           100
#define DEBOUNCE_TIME_MS       50
#define AUTOCYCLE_WAIT_TIME_MS 1000

typedef struct {
  uint8_t addr;      // device address to send the command to
  uint8_t reg;       // register to write to
  uint8_t val;       // value to write
  uint32_t delay_ms; // delay (in milliseconds) to wait before sending this command
} device_command_t;

typedef struct {
  uint8_t addr;
  uint8_t reg;
} device_register_t;

extern const uint NUM_SWITCHES;
extern const uint SWITCHES[];

extern const uint NUM_FEATURE_SWITCHES;
extern const uint FEATURE_SWITCHES[];

extern const uint NUM_BIT_SWITCHES;
extern const uint BIT_SWITCHES[];

extern const uint NUM_TEST_REGISTERS;
extern const device_register_t TEST_REGISTERS[];

#endif