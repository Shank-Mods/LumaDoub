#include "features.h"

int set_input(input_t *current_input, bool *autocycle) {
  int ret;

  bool cvbs_in = !gpio_get(SWITCH_CVBS_IN);                 // active low
  bool yc_in = !gpio_get(SWITCH_YC_IN);                     // active low
  bool test_pattern_in = !gpio_get(SWITCH_TEST_PATTERN_IN); // active low

  if (!cvbs_in && !yc_in) {
    ret = send_command_ypbpr_input();
    if (ret < 0)
      return ret;

    *current_input = YPBPR;
    *autocycle = false;

  } else if (cvbs_in && !yc_in) {
    ret = send_command_cvbs_input();
    if (ret < 0)
      return ret;

    *current_input = CVBS;
    *autocycle = false;

  } else if (!cvbs_in && yc_in) {
    ret = send_command_yc_input();
    if (ret < 0)
      return ret;

    *current_input = YC;
    *autocycle = false;

  } else if (test_pattern_in) {
    ret = send_command_test_pattern_input();
    if (ret < 0)
      return ret;

    *current_input = TEST;
    *autocycle = false;

  } else {
    *autocycle = true;
  }

  return 0;
};

int cycle_input(input_t *current_input, input_t *next_input) {
  int ret;

  printf("\tcycle_input: current: %d, next: %d ", *current_input, *next_input);

  if (*next_input == YPBPR) {
    printf(" |  trying ypbpr ");
    // try YPbPr
    ret = send_command_ypbpr_input();
    if (ret < 0)
      return ret;

    *current_input = YPBPR;
    *next_input = CVBS;

  } else if (*next_input == CVBS) {
    printf(" |  trying cvbs ");
    // try CVBS
    ret = send_command_cvbs_input();
    if (ret < 0)
      return ret;

    *current_input = CVBS;
    *next_input = YC;

  } else if (*next_input == YC) {
    printf(" |  trying yc ");
    // try YC
    ret = send_command_yc_input();
    if (ret < 0)
      return ret;

    *current_input = YC;
    *next_input = YPBPR;
  }

  return 0;
};

int set_output() {
  int ret;
  bool ypbpr_out = !gpio_get(SWITCH_YPBPR_OUT);        // active low
  bool cvbs_out = !gpio_get(SWITCH_CVBS_OUT);          // active low
  bool rgb_sync_disabled = !gpio_get(SWITCH_RGB_SYNC); // active low

  if (ypbpr_out) {
    ret = send_command_ypbpr_output();
  } else if (cvbs_out) {
    ret = send_command_cvbs_output();
  } else if (rgb_sync_disabled) {
    ret = send_command_rgb_output_no_sync();
  } else {
    ret = send_command_rgb_output_rsgsbs();
  }

  return ret;
};

int set_line_double() {
  int ret;
  bool line_double = !gpio_get(SWITCH_LINE_DOUBLE); // active low

  if (line_double) {
    ret = send_command_line_double();
  } else {
    ret = send_command_line_undouble();
  }

  return ret;
};

int set_vaseline_filter() {
  int ret;
  bool vaseline_on = !gpio_get(SWITCH_VASELINE); // active low

  if (vaseline_on) {
    ret = send_command_vaseline_on();
  } else {
    ret = send_command_vaseline_off();
  }

  return ret;
};

int set_notch_filter() {
  int ret;
  bool notch_filter = !gpio_get(SWITCH_NOTCH_FILTER); // active low

  if (notch_filter) {
    ret = send_command_notch_filter_on();
  } else {
    ret = send_command_notch_filter_off();
  }

  return ret;
};

int set_force_240p() {
  int ret;
  bool force_240p = !gpio_get(SWITCH_FORCE_240P); // active low
  bool is_interlaced;

  ret = get_is_interlaced(&is_interlaced);
  if (ret < 0)
    return ret;

  if (force_240p || !is_interlaced) {
    ret = send_command_force_240p();
  } else {
    ret = send_command_unforce_240p();
  }
};

int set_pbpr_sync() {
  int ret;
  bool pbpr_sync = !gpio_get(SWITCH_PBPR_SYNC); // active low

  if (pbpr_sync) {
    ret = send_command_pbpr_sync_on();
  } else {
    ret = send_command_pbpr_sync_off();
  }

  return ret;
};

int set_beta_input_voltage() {
  int ret;
  bool beta_input_voltage = !gpio_get(SWITCH_BETA_INPUT_VOLTAGE); // active low

  if (beta_input_voltage) {
    ret = send_command_beta_input_voltage_on();
  } else {
    ret = send_command_beta_input_voltage_off();
  }

  return ret;
};

int set_pedestal_output() {
  int ret;
  bool pedestal_output = !gpio_get(SWITCH_PEDESTAL_OUTPUT); // active low

  if (pedestal_output) {
    ret = send_command_pedestal_output_on();
  } else {
    ret = send_command_pedestal_output_off();
  }

  return ret;
};

int set_custom_registers() {
  int ret;
  bool bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0;

  bit0 = !gpio_get(SWITCH_B0); // active low
  bit1 = !gpio_get(SWITCH_B1); // active low
  bit2 = !gpio_get(SWITCH_B2); // active low
  bit3 = !gpio_get(SWITCH_B3); // active low
  bit4 = !gpio_get(SWITCH_B4); // active low
  bit5 = !gpio_get(SWITCH_B5); // active low
  bit6 = !gpio_get(SWITCH_B6); // active low
  bit7 = !gpio_get(SWITCH_B7); // active low

  uint8_t value = ((bit7 << 7) | (bit6 << 6) | (bit5 << 5) | (bit4 << 4) | (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | (bit0 << 0));

  for (int i = 0; i < NUM_TEST_REGISTERS; i++) {
    ret = write_custom_register_value(TEST_REGISTERS[i].addr, TEST_REGISTERS[i].reg, value);
    if (ret < 0)
      return ret;
  }

  return 0;
};
