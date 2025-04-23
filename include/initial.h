#include <stdint.h>
#include <stdlib.h>

#ifndef INITIAL_H
#define INITIAL_H

void init_leds(size_t led_count);
void init_buttons(size_t button_count);
void wait_for_calibration();
void wait_for_start();
void button_interrupt_callback(uint32_t event_mask);

#endif