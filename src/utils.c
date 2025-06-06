#include "utils.h"

#include <hardware/i2c.h>
#include <hardware/sync.h>
#include <hardware/watchdog.h>
#include <pico/error.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "program_state.h"

void init_led_pin(uint8_t pin)
{
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_OUT);
}

void init_button_pin(uint8_t pin)
{
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_IN);
	gpio_pull_up(pin);
}

bool button_pressed(uint8_t pin)
{
	return gpio_get(pin) == 0;	// Active low
}

void i2c_init_custom(i2c_inst_t *i2c, uint16_t sda_pin, uint16_t scl_pin,
					 uint32_t baudrate)
{
	i2c_init(i2c, baudrate);

	gpio_set_function(sda_pin, GPIO_FUNC_I2C);
	gpio_set_function(scl_pin, GPIO_FUNC_I2C);

	gpio_pull_up(sda_pin);
	gpio_pull_up(scl_pin);
}

void clear_terminal()
{
	// ANSI escape code to clear screen and move cursor to top-left
	printf("\033[2J\033[H");
	fflush(stdout);
}

bool get_user_input(char *input, size_t input_size)
{
	int32_t c = getchar_timeout_us(0);
	if (c == PICO_ERROR_TIMEOUT)
	{
		return false;
	}
	size_t i = 0;

	putchar(c);
	if (c == '\r' || c == '\n')
	{
		return false;
	}

	input[i++] = c;

	while (i < input_size)
	{
		c = getchar();
		putchar(c);

		if (c == '\r' || c == '\n')
		{
			break;
		}

		input[i++] = c;
	}

	input[i] = '\0';
	return true;
}

char *str_concat(const char *str1, const char *str2)
{
	if (!str1 || !str2)
	{
		return NULL;
	}

	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	char *result = (char *) malloc((len1 + len2) + 1);
	if (!result)
	{
		return NULL;
	}

	uint32_t pos = 0;
	for (size_t i = 0; i < len1; i++)
	{
		result[pos++] = str1[i];
	}
	for (size_t j = 0; j < len2; j++)
	{
		result[pos++] = str2[j];
	}
	result[pos] = '\0';

	return result;
}
