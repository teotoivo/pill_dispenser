#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <stdbool.h>
#include <stdint.h>

#include "program_state.h"

#define CALIBRATION_RUNS 1

/* Maximum time (ms) we wait for a pill to reach the piezo sensor */
#define PILL_DROP_TIMEOUT_MS 1500

/* How many times to blink error LED when dispensing fails */
#define ERROR_BLINK_COUNT 5U

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

bool dispense_next_pill(ProgramState *program_state);

void piezo_interrupt_callback(uint32_t event_mask);

void dispense_next_pill_with_confirmation(ProgramState *program_state,
										  uint8_t tries);
#endif	// !STEPPER_MOTOR_H
