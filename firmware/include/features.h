#ifndef FEATURES_H
#define FEATURES_H

#include "commands.h"
#include "defines.h"

#include "hardware/gpio.h" // TODO: pass switch values as parameters instead and remove this dependency?
#include "pico/stdlib.h"

typedef enum { TEST, YPBPR, CVBS, YC } input_t; // TODO: move this to defines.h?

int set_input(input_t *current_input, bool *is_autocycle_enabled, bool *autocycle);
int cycle_input(input_t *current_input, input_t *next_input);
int set_output();
int set_line_double();
int set_vaseline_filter();
int set_notch_filter();
int set_force_240p();
int set_pbpr_sync();
int set_beta_input_voltage();
int set_pedestal_output();
int set_test_pattern();
int set_custom_registers();

#endif