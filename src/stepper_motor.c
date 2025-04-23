#include "stepper_motor.h"

#include <hardware/gpio.h>
#include <pico/util/queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware_definitions.h"
#include "program_state.h"

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

	// TODO CHECK IF IT IS ALLREADY CALIBRATED
	program_state->steps_per_rev = 0;  // sets to uncalbrated
}

void calibarate_stepper_motor(ProgramState *program_state)
{
	printf("Calibrating\n");
	program_state->is_running = 1;
	write_program_state(program_state);

	// clear the event que before calibration
	int event;
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
	printf("Found start\n");

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

	program_state->absolute_motor_position = 0;
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
			program_state->steps_per_rev =
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
		int event = 1;
		queue_try_add(&opto_fork_events, &event);
	}
}
