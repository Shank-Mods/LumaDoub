#ifndef COMMANDS_H
#define COMMANDS_H

#include "defines.h"
#include "i2c.h"

#include "pico/stdlib.h"
#include <stdio.h> // TODO: remove this!

int send_command_fake_scannies_on();
int send_command_fake_scannies_off();
int send_command_freerun_480i_60Hz_YPbPr_out();
int send_command_7184_rgb_test();
int send_command_cvbs_input();
int send_command_yc_input();
int send_command_ypbpr_input();
int send_command_test_pattern_input();
int send_command_rgb_output_rsgsbs();
int send_command_rgb_output_no_sync();
int send_command_ypbpr_output();
int send_command_cvbs_output();
int send_command_line_double();
int send_command_line_undouble();
int send_command_vaseline_on();
int send_command_vaseline_off();
int send_command_notch_filter_on();
int send_command_notch_filter_off();
int send_command_force_240p();
int send_command_unforce_240p();
int send_command_pbpr_sync_on();
int send_command_pbpr_sync_off();
int send_command_beta_input_voltage_on();
int send_command_beta_input_voltage_off();
int send_command_pedestal_output_on();
int send_command_pedestal_output_off();
int enter_vdp_map();
int enable_hlock_interrupt();
int clear_hlock_interrupt();
int exit_vdp_map();

int get_is_interlaced(bool *is_interlaced);
int get_lock_status(bool *is_inlock_set, bool *is_hlock_set);
int get_hlock_changed(bool *is_hlock_changed);

int write_custom_register_value(uint8_t device, uint8_t reg, uint8_t val);

#endif