#ifndef UTILS_H
#define UTILS_H
#include <hardware/i2c.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "program_state.h"

void init_button_pin(uint8_t pin);
void init_led_pin(uint8_t pin);
bool button_pressed(uint8_t pin);
void i2c_init_custom(i2c_inst_t *i2c, uint16_t sda_pin, uint16_t scl_pin,
					 uint32_t baudrate);
void clear_terminal();
bool get_user_input(char *input, size_t input_size);
void restart_board(void);
char *str_concat(const char *str1, const char *str2);

void main_things(ProgramState *program_state);
#endif
