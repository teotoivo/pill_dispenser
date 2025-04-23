#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stdio.h>

#include "eeprom.h"
#include "hardware_definitions.h"
#include "program_state.h"
#include "stepper_motor.h"
#include "utils.h"
#include "initial.h"

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

int main()
{
	stdio_init_all();  // needed for uart
	clear_terminal();


	// add intterupts here must always call irq_callback add the callback inside
	// the callback
	gpio_set_irq_enabled_with_callback(OPTO_FORK, GPIO_IRQ_EDGE_FALL, true,
									   &irq_callback);
	gpio_set_irq_enabled_with_callback(SW_1, GPIO_IRQ_EDGE_RISE, true, &irq_callback);

	ProgramState program_state = {
		.steps_per_rev = 0,
	};

	// add intit functions here
  i2c_init_custom(EEPROM_PORT, EEPROM_sda_pin, EEPROM_scl_pin,
					EEPROM_BAUD_RATE);
	init_stepper_motor(&program_state);
	init_leds(LED_COUNT);
	init_buttons(BUTTON_COUNT);

	

	

	// dont move this
	load_program_state(&program_state);

wait_for_calibration();
	// add calibration function here
	wait_for_start();
  
	while (true)
	{
		printf("Working\n");
	}
}
