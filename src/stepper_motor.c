#include "stepper_motor.h"

#include <hardware/gpio.h>
#include <pico/time.h>
#include <pico/util/queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware_definitions.h"
#include "lora.h"
#include "program_state.h"
#include "utils.h"

// Half-step sequence for the stepper motor
static const uint8_t STEP_SEQUENCE[8][4] = {
	{1, 0, 0, 0},  // Step 1
	{1, 1, 0, 0},  // Step 2
	{0, 1, 0, 0},  // Step 3
	{0, 1, 1, 0},  // Step 4
	{0, 0, 1, 0},  // Step 5
	{0, 0, 1, 1},  // Step 6
	{0, 0, 0, 1},  // Step 7
	{1, 0, 0, 1}   // Step 8
};

static queue_t opto_fork_events;
static queue_t piezo_events;

static uint8_t microstep_index =
	0;	// tracs where the moto is inside the sequence from 0 - 7

void init_stepper_pin(uint8_t pin)
{
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_OUT);
}

void init_stepper_motor(ProgramState *program_state)
{
	// Configure the opto fork input with pull-up.
	gpio_init(OPTO_FORK);
	gpio_set_dir(OPTO_FORK, GPIO_IN);
	gpio_pull_up(OPTO_FORK);

	// Configure stepper controller pins as outputs.
	init_stepper_pin(STEPPER_CONTROLLER_0);
	init_stepper_pin(STEPPER_CONTROLLER_1);
	init_stepper_pin(STEPPER_CONTROLLER_2);
	init_stepper_pin(STEPPER_CONTROLLER_3);

	queue_init(&opto_fork_events, sizeof(uint8_t), 20);
	queue_init(&piezo_events, sizeof(uint8_t), 20);
}

void reset_stepper_motor_offset(ProgramState *program_state)
{
	program_state->is_running = 1;
	write_program_state(program_state);

	uint8_t event;
	while (queue_try_remove(&opto_fork_events, &event));

	while (true)
	{
		if (queue_try_remove(&opto_fork_events, &event))
		{
			break;
		}

		run_stepper_steps(program_state, -1, false);
	}

	run_stepper_steps(program_state, -150, false);

	program_state->absolute_motor_position = 0;
	program_state->is_running			   = 0;
	write_program_state(program_state);

	uint8_t right_state			= program_state->current_pill;
	program_state->current_pill = 0;
	while (program_state->current_pill < right_state)
	{
		dispense_next_pill(program_state);
	}
}

void calibarate_stepper_motor(ProgramState *program_state)
{
	program_state->is_running = 1;
	write_program_state(program_state);

	// clear the event que before calibration
	uint8_t event;
	while (queue_try_remove(&opto_fork_events, &event));

	uint32_t total_steps = 0;
	while (true)
	{
		if (queue_try_remove(&opto_fork_events, &event))
		{
			break;	// found start pos
		}
		run_stepper_steps(program_state, 1, false);
	}

	for (uint16_t i = 0; i < CALIBRATION_RUNS; i++)
	{
		uint32_t steps = 0;
		while (true)
		{
			if (queue_try_remove(&opto_fork_events, &event))
				break;	// found calib position go to next calibration run

			run_stepper_steps(program_state, 1, false);
			steps++;
		}
		total_steps += steps;
	}

	run_stepper_steps(program_state, 150, false);  // fix offset

	program_state->absolute_motor_position = 0;
	program_state->current_pill			   = 0;
	program_state->steps_per_rev =
		total_steps / CALIBRATION_RUNS;	 // averaage over the runs

	program_state->is_running = 0;
	write_program_state(program_state);
}

void run_stepper_steps(ProgramState *program_state, int32_t steps_to_run,
					   bool mark_in_eeprom)
{
	if (mark_in_eeprom)
	{
		program_state->is_running = 1;
		write_program_state(program_state);
	}

	const int32_t SLEEP_DELAY = 1000;  // the speed the motor runs at
	if (steps_to_run > 0)
	{
		for (int i = 0; i < steps_to_run; i++)
		{
			program_state->absolute_motor_position =
				(program_state->absolute_motor_position + 1) %
				program_state->steps_per_rev;
			microstep_index = (microstep_index + 1) % 8;
			gpio_put(STEPPER_CONTROLLER_0, STEP_SEQUENCE[microstep_index][0]);
			gpio_put(STEPPER_CONTROLLER_1, STEP_SEQUENCE[microstep_index][1]);
			gpio_put(STEPPER_CONTROLLER_2, STEP_SEQUENCE[microstep_index][2]);
			gpio_put(STEPPER_CONTROLLER_3, STEP_SEQUENCE[microstep_index][3]);

			sleep_us(SLEEP_DELAY);
		}
	}
	else if (steps_to_run < 0)
	{
		for (int i = 0; i < labs(steps_to_run); i++)
		{
			microstep_index = (microstep_index - 1 + 8) % 8;
			program_state->absolute_motor_position =
				(program_state->absolute_motor_position - 1 +
				 program_state->steps_per_rev) %
				program_state->steps_per_rev;
			gpio_put(STEPPER_CONTROLLER_0, STEP_SEQUENCE[microstep_index][0]);
			gpio_put(STEPPER_CONTROLLER_1, STEP_SEQUENCE[microstep_index][1]);
			gpio_put(STEPPER_CONTROLLER_2, STEP_SEQUENCE[microstep_index][2]);
			gpio_put(STEPPER_CONTROLLER_3, STEP_SEQUENCE[microstep_index][3]);

			sleep_us(SLEEP_DELAY);
		}
	}

	// mark_in_eeprom
	if (mark_in_eeprom)
	{
		program_state->is_running = 0;
		write_program_state(program_state);
	}
}

void opto_fork_interrupt_callback(uint32_t event_mask)
{
	if ((event_mask & GPIO_IRQ_EDGE_FALL))
	{
		uint8_t event = 1;
		queue_try_add(&opto_fork_events, &event);
	}
}

void dispense_next_pill(ProgramState *program_state)
{
	program_state->is_running = 1;
	write_program_state(program_state);

	run_stepper_steps(program_state, program_state->steps_per_rev / PILL_SLOTS,
					  false);

	program_state->current_pill += 1;
	if (program_state->current_pill == PILL_SLOTS - 1)
	{
		program_state->current_pill			   = 0;
		program_state->is_running			   = 0;
		program_state->absolute_motor_position = 0;
		program_state->steps_per_rev		   = 0;
		printf("restart\n");
		write_program_state(program_state);
		send_message("PILL EMPTY", program_state->is_lora_connected);
		main_things(program_state);
	}

	program_state->is_running = 0;
	write_program_state(program_state);
}

void piezo_interrupt_callback(uint32_t event_mask)
{
	if ((event_mask & GPIO_IRQ_EDGE_FALL))
	{
		uint8_t event = 1;
		queue_try_add(&piezo_events, &event);
	}
}

static bool wait_for_piezo_event(uint32_t timeout_ms)
{
	absolute_time_t deadline = make_timeout_time_ms(timeout_ms);
	uint8_t dummy;

	/* Poll until an event arrives or timeout expires */
	while (!time_reached(deadline))
	{
		if (queue_try_remove(&piezo_events, &dummy))
		{
			return true; /* At least one falling edge detected */
		}
	}
	return false;
}

static void blink_error_led(void)
{
	for (uint32_t i = 0; i < ERROR_BLINK_COUNT; i++)
	{
		gpio_put(LED_D3, 1);
		sleep_ms(250);
		gpio_put(LED_D3, 0);
		sleep_ms(250);
	}
}

void dispense_next_pill_with_confirmation(ProgramState *program_state,
										  uint8_t tries)
{
	// discard possible previousinterrupts
	uint8_t dummy;
	while (queue_try_remove(&piezo_events, &dummy));

	dispense_next_pill(program_state);

	const bool pill_detected = wait_for_piezo_event(PILL_DROP_TIMEOUT_MS);

	if (!pill_detected)
	{
		send_message("NOT DISPENSED", program_state->is_lora_connected);
		blink_error_led();
		dispense_next_pill_with_confirmation(program_state, tries + 1);
	}
	else
	{
		send_message("DISPENSED", program_state->is_lora_connected);
	}
}
