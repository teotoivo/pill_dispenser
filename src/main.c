#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stdio.h>

#include "globals.h"
#include "hardware_definitions.h"
#include "stepper_motor.h"
#include "utils.h"

void irq_callback(uint gpio, uint32_t event_mask)
{
	switch (gpio)
	{
		case OPTO_FORK:
			opto_fork_interrupt_callback(event_mask);
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

	ProgramState program_state = {
		.steps_per_rev = 0,
	};

	// add intit functions here
	init_stepper_motor(&program_state);

	while (true)
	{
	}
}
