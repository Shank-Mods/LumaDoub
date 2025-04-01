#include "commands.h"
#include "defines.h"
#include "features.h"
#include "i2c.h"

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h" // TODO: don't include twice?
#include "pico/sync.h"
#include "pico/time.h"
#include <stdio.h>

volatile bool fake_scannies = 1; // Variable to store bit state
input_t current_input, next_input;
volatile bool autocycle;
int error = 0; // Variable to hold error codes from interrupt handlers
critical_section_t irq_critical_section;
alarm_id_t switch_alarm, hlock_alarm, autocycle_alarm;

void init();
void set_error(int err);
void switch_callback(uint gpio);
void autocycle_inputs();
void enable_interrupts();
void gpio_callback(uint gpio, uint32_t events);
void update_switch_settings();
void cycle_inputs(input_t *current_input, input_t *next_input, bool *autocycle);

int main() {
  init();

  current_input = YPBPR;
  next_input = YPBPR;

  update_switch_settings();
  enable_interrupts();

  while (true) {
    // printf("main loop: autocycle: %d\n", autocycle);

    if (autocycle) {
      sleep_ms(LOOP_RATE_MS);
      cycle_inputs(&current_input, &next_input, &autocycle);
      printf("\tmain loop: current: %d, next: %d, autocycle: %d\n", current_input, next_input, autocycle);
    }

    if (error < 0)
      return error;

    // tight_loop_contents();
  }

  return 0;
}

void init() {
  critical_section_init(&irq_critical_section);

  alarm_pool_init_default();

  stdio_init_all();

  i2c_init(I2C_PORT, 100 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

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

  // int ret = send_command_7184_rgb_test();
  // if (ret < 0)
  //   return set_error(ret); // TODO: don't

  int ret = send_command_freerun_480i_60Hz_YPbPr_out();
  if (ret < 0)
    return set_error(ret); // TODO: don't

  ret = send_command_ypbpr_input();
  if (ret < 0)
    return set_error(ret); // TODO: don't

  for (int i = 0; i < NUM_SWITCHES; i++) {
    gpio_init(SWITCHES[i]);
    gpio_set_dir(SWITCHES[i], GPIO_IN);
    gpio_pull_up(SWITCHES[i]);
  }

  gpio_init(HSyncIn);
  gpio_set_dir(HSyncIn, GPIO_IN);
};

void update_switch_settings() {
  int ret;

  // input (or autocycle)
  ret = set_input(&current_input, &autocycle);
  if (ret < 0)
    return set_error(ret);

  printf("input: ");
  if (current_input == YPBPR) {
    printf("ypbpr ");
  } else if (current_input == CVBS) {
    printf("cvbs  ");
  } else if (current_input == YC) {
    printf("yc    ");
  }

  // if (autocycle)
  //   printf(" |  ready to cycle\n");
  // else
  //   printf(" |  manual\n");

  // output
  ret = set_output();
  if (ret < 0)
    return set_error(ret);

  // force 240p
  ret = set_force_240p();
  if (ret < 0)
    return set_error(ret);

  // line double
  ret = set_line_double();
  if (ret < 0)
    return set_error(ret);

  // vaseline filter
  ret = set_vaseline_filter();
  if (ret < 0)
    return set_error(ret);

  // notch filter
  ret = set_notch_filter();
  if (ret < 0)
    return set_error(ret);

  // pbpr sync
  ret = set_pbpr_sync();
  if (ret < 0)
    return set_error(ret);

  // beta input voltage
  ret = set_beta_input_voltage();
  if (ret < 0)
    return set_error(ret);

  // pedestal output
  ret = set_pedestal_output();
  if (ret < 0)
    return set_error(ret);

  // custom register
  // ret = set_custom_registers();
  // if (ret < 0)
  //   return set_error(ret);

  // if (autocycle) {
  //   autocycle_inputs();
  // }

  return;
};

int check_signal_validity(input_t current_input, bool *is_valid) {
  bool inlock, hlock;
  bool valid_ypbpr, valid_yc, valid_cvbs;
  int ret;

  critical_section_enter_blocking(&irq_critical_section);

  ret = get_lock_status(&inlock, &hlock);
  if (ret < 0)
    return ret;

  printf("inlock: %d, hlock: %d\n", inlock, hlock);

  valid_ypbpr = (current_input == YPBPR && hlock);
  valid_yc = (current_input == YC && hlock);
  valid_cvbs = (current_input == CVBS && hlock);

  critical_section_exit(&irq_critical_section);

  *is_valid = (valid_ypbpr || valid_yc || valid_cvbs);
  return 0;
};

void cycle_inputs(input_t *current_input, input_t *next_input, bool *autocycle) {
  bool is_signal_valid;
  int ret;

  ret = check_signal_validity(*current_input, &is_signal_valid);
  if (ret < 0)
    return set_error(ret);

  // if we don't have hlock, cycle input once
  if (!is_signal_valid) {
    ret = cycle_input(current_input, next_input);
    return set_error(ret);
  }

  // // else, wait one cycle...
  // sleep_ms(LOOP_RATE_MS);

  // // ...then check again to be sure
  // ret = check_signal_validity(*current_input, &is_signal_valid);
  // if (ret < 0)
  //   return set_error(ret);

  // // if we lost hlock, cycle input once anyway
  // if (!is_signal_valid) {
  //   ret = cycle_input(current_input, next_input);
  //   return set_error(ret);
  // }

  // else, assume signal really is valid
  *next_input = YPBPR;
  *autocycle = false;

  printf("input: ");
  if (*current_input == YPBPR) {
    printf("ypbpr ");
  } else if (*current_input == CVBS) {
    printf("cvbs  ");
  } else if (*current_input == YC) {
    printf("yc    ");
  }

  printf("\n");
};

void autocycle_inputs() {
  printf("entered autocycle function\n");

  while (true) {
    printf("entered autocycle loop\n");

    // sleep_ms(500); // TODO: replace this with busy_wait_ms
    cycle_inputs(&current_input, &next_input, &autocycle);
  }
};

void hlock_interrupt_clear() {
  int ret;

  // printf("entered hlock interrupt clear alarm callback\n");
  critical_section_enter_blocking(&irq_critical_section);
  // printf("entered hlock interrupt clear alarm critical section\n");

  ret = enter_vdp_map();
  if (ret < 0)
    return set_error(ret);

  ret = clear_hlock_interrupt();
  if (ret < 0)
    return set_error(ret);

  ret = enable_hlock_interrupt();
  if (ret < 0)
    return set_error(ret);

  ret = exit_vdp_map();
  if (ret < 0)
    return set_error(ret);

  // printf("exiting hlock interrupt clear alarm critical section\n");
  critical_section_exit(&irq_critical_section);
  printf("cleared interrupts\n");
};

int64_t autocycle_alarm_callback(alarm_id_t alarm_id, void *user_data) {
  // printf("\tentering hlock alarm callback\n");
  autocycle = true;
  // printf("\tchecked hlock\n");
  autocycle_alarm = 0;
  // printf("\tcleared hlock alarm\n");
  return 0;
};

void schedule_autocycle() {
  // printf("\tentering hlock callback\n");
  critical_section_enter_blocking(&irq_critical_section);
  // printf("\tentered hlock callback critical section\n");

  cancel_alarm(autocycle_alarm);
  autocycle_alarm = add_alarm_in_ms(AUTOCYCLE_WAIT_TIME_MS, autocycle_alarm_callback, NULL, true);

  // if (autocycle_alarm <= 0) {
  //   autocycle_alarm = add_alarm_in_ms(AUTOCYCLE_WAIT_TIME_MS, autocycle_alarm_callback, NULL, true);
  //   // printf("\t\tadded new alarm\n");
  // } else {
  //   // printf("\t\tno alarm added\n");
  // }

  critical_section_exit(&irq_critical_section);
  // printf("\texited hlock callback critical section\n");
};

void check_hlock() {
  int ret;
  bool hlock_changed;

  printf("entered hlock callback\n");
  critical_section_enter_blocking(&irq_critical_section);
  // printf("entered hlock critical section\n");

  ret = enter_vdp_map();
  if (ret < 0)
    return set_error(ret);

  ret = get_hlock_changed(&hlock_changed);
  if (ret < 0)
    return set_error(ret);

  ret = clear_hlock_interrupt();
  if (ret < 0)
    return set_error(ret);

  ret = exit_vdp_map();
  if (ret < 0)
    return set_error(ret);

  // ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x13, &read_value4);
  // if (ret < 0)
  //   return set_error(ret);

  // bool hlock_changed = read_value5 & 0b00000100;
  // bool hlock_from_main = read_value4 & 0b00000001;

  // printf("exiting hlock critical section\n");
  critical_section_exit(&irq_critical_section);
  // printf("exited hlock critical section  |  ");

  printf("hlock changed: %d\n", hlock_changed);

  // if (/*!hlock_from_main &&*/ hlock_changed)
  // autocycle = true;
  schedule_autocycle();

  // schedule_hlock_interrupt_clear();

  return;
};

void enable_interrupts() {
  int ret;

  gpio_set_irq_callback(&gpio_callback);
  irq_set_enabled(IO_IRQ_BANK0, true);

  for (int i = 0; i < NUM_FEATURE_SWITCHES; i++) {
    gpio_set_irq_enabled(FEATURE_SWITCHES[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
  }

  for (int i = 0; i < NUM_BIT_SWITCHES; i++) {
    gpio_set_irq_enabled(BIT_SWITCHES[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
  }

  gpio_set_irq_enabled(HSyncIn, GPIO_IRQ_EDGE_FALL, true);

  critical_section_enter_blocking(&irq_critical_section);
  ret = enter_vdp_map();
  if (ret < 0)
    return set_error(ret);

  ret = enable_hlock_interrupt(); // TODO: must be in VDP map to do this!
  if (ret < 0)
    return set_error(ret);

  ret = clear_hlock_interrupt(); // TODO: must be in VDP map to do this! (and wasn't before.) COULD BE CAUSE OF LATEST ERROR?
  if (ret < 0)
    return set_error(ret);

  ret = exit_vdp_map();
  if (ret < 0)
    return set_error(ret);
  critical_section_exit(&irq_critical_section);
};

int64_t hlock_alarm_callback(alarm_id_t alarm_id, void *user_data) {
  // printf("\tentering hlock alarm callback\n");
  check_hlock();
  // printf("\tchecked hlock\n");
  hlock_alarm = 0;
  // printf("\tcleared hlock alarm\n");
  return 0;
};

void hlock_callback() {
  // printf("\tentering hlock callback\n");
  critical_section_enter_blocking(&irq_critical_section);
  // printf("\tentered hlock callback critical section\n");

  if (hlock_alarm <= 0) {
    hlock_alarm = add_alarm_in_ms(DEBOUNCE_TIME_MS, hlock_alarm_callback, NULL, true);
    // printf("\t\tadded new alarm\n");
  } else {
    // printf("\t\tno alarm added\n");
  }

  critical_section_exit(&irq_critical_section);
  // printf("\texited hlock callback critical section\n");
};

int64_t switch_alarm_callback(alarm_id_t alarm_id, void *user_data) {
  // printf("\tentering switch alarm callback\n");
  update_switch_settings();
  // printf("\tupdated switch settings\n");
  switch_alarm = 0;
  // printf("\tcleared switch alarm\n");
  return 0;
};

void switch_callback(uint gpio) {
  // printf("\tentering switch callback\n");
  critical_section_enter_blocking(&irq_critical_section);
  // printf("\tentered switch callback critical section\n");

  if (switch_alarm <= 0) {
    switch_alarm = add_alarm_in_ms(DEBOUNCE_TIME_MS, switch_alarm_callback, NULL, true);
    // printf("\t\tadded new alarm\n");
  } else {
    // printf("\t\tno alarm added\n");
  }

  critical_section_exit(&irq_critical_section);
  // printf("\texited switch callback critical section\n");
};

void gpio_callback(uint gpio, uint32_t events) {
  // printf("entered gpio callback\n");
  switch (gpio) {
  case HSyncIn:
    printf("triggering hlock callback\n");
    hlock_callback();
    break;
  default:
    printf("triggering switch callback\n");
    switch_callback(gpio);
    break;
  }
};

void set_error(int err) {
  if (err >= 0)
    return;

  printf("error is %d\n", err);
  error = err;
};