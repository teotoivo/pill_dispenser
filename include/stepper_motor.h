#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <stdint.h>
#include <stdbool.h>

#include "program_state.h"

#define CALIBRATION_RUNS 1

/**
 * initializes everything needed for the stepper motor
 * main still needs to set interrupt callback
 */
void init_stepper_motor(ProgramState *program_state);

/**
 * runs stepper motot for steps_to_run steps
 * mark_in_eeprom is mainly for calibration since it handles marking it self
 */
void run_stepper_steps(ProgramState *program_state, int32_t steps_to_run,
					   bool mark_in_eeprom);

void opto_fork_interrupt_callback(uint32_t event_mask);


/**
 * runs stepper motor backwards incase of offset after pill dispension
 */
void reset_stepper_motor_offset(ProgramState *program_state);

void calibarate_stepper_motor(ProgramState *program_state);

#endif	// !STEPPER_MOTOR_H
