#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stdio.h>

#include "eeprom.h"
#include "hardware_definitions.h"
#include "initial.h"
#include "lora.h"
#include "program_state.h"
#include "stepper_motor.h"
#include "utils.h"

void irq_callback(uint gpio, uint32_t event_mask)
{
	switch (gpio)
	{
		case OPTO_FORK:
			opto_fork_interrupt_callback(event_mask);
			break;
		case SW_1:
			button_interrupt_callback(event_mask);
			break;
	}
}

void main_things(ProgramState *program_state)
{
	// dont move this
	load_program_state(program_state);

	if (program_state->is_running == 1)
	{
		reset_stepper_motor_offset(program_state);
	}
	if (program_state->steps_per_rev == 0)
	{
		wait_for_calibration();
		calibarate_stepper_motor(program_state);
		wait_for_start();
	}

	while (true)
	{
		dispense_next_pill(program_state);
		sleep_ms(30 * 100);	 // 30 sec
	}
}

int main()
{
	stdio_init_all();  // needed for uart
	clear_terminal();

	// add intterupts here must always call irq_callback add the callback inside
	// the callback
	gpio_set_irq_enabled_with_callback(OPTO_FORK, GPIO_IRQ_EDGE_FALL, true,
									   &irq_callback);
	gpio_set_irq_enabled_with_callback(SW_1, GPIO_IRQ_EDGE_RISE, true,
									   &irq_callback);

	ProgramState program_state;

	// add intit functions here
	i2c_init_custom(EEPROM_PORT, EEPROM_sda_pin, EEPROM_scl_pin,
					EEPROM_BAUD_RATE);
	init_stepper_motor(&program_state);
	init_leds(LED_COUNT);
	init_buttons(BUTTON_COUNT);

	// Program is on connecting to lora
	gpio_put(LED_D2, true);

	bool connected_to_lora = connect_to_lora_module();
	if (connected_to_lora)
	{
		// Connected to lora
		send_message("BOOT");
		gpio_put(LED_D2, false);
	}

	main_things(&program_state);
}
