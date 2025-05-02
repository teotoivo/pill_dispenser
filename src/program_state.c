#include "program_state.h"

#include <stdint.h>
#include <stdio.h>

#include "eeprom.h"

void load_program_state(ProgramState* program_state)
{
	// steps_per_rev
	uint16_t steps		   = eeprom_read_u16(EE_ADDR_STEPS_PER_REV);
	uint16_t steps_inverse = eeprom_read_u16(EE_ADDR_STEPS_PER_REV + 2);

	if (steps == (uint16_t) (~steps_inverse))
	{
		program_state->steps_per_rev = steps;
	}
	else
	{
		program_state->steps_per_rev = 0;
	}

	// absolute_motor_position
	uint16_t abs_pos	 = eeprom_read_u16(EE_ADDR_ABS_POSITION);
	uint16_t abs_pos_inv = eeprom_read_u16(EE_ADDR_ABS_POSITION + 2);

	if (abs_pos == ~abs_pos_inv)
		program_state->absolute_motor_position = abs_pos;

	// is_running
	uint8_t run_flag	 = eeprom_read_byte(EE_ADDR_IS_RUNNING);
	uint8_t run_flag_inv = eeprom_read_byte(EE_ADDR_IS_RUNNING + 1);

	if ((uint8_t) (run_flag ^ run_flag_inv) == 0xFF)
		program_state->is_running = run_flag;
	else
		program_state->is_running = 0;

	// current_pill
	uint8_t current_pill	 = eeprom_read_byte(EE_ADDR_CURRENT_PILL);
	uint8_t current_pill_inv = eeprom_read_byte(EE_ADDR_CURRENT_PILL + 1);

	if ((uint8_t) (current_pill ^ current_pill_inv) == 0xFF)
		program_state->current_pill = current_pill;
}

void write_program_state(ProgramState* program_state)
{
	// steps_per_rev
	uint16_t steps	   = program_state->steps_per_rev;
	uint16_t steps_inv = ~steps;

	eeprom_write_u16(EE_ADDR_STEPS_PER_REV + 2, steps_inv);
	eeprom_write_u16(EE_ADDR_STEPS_PER_REV, steps);

	// absolute_motor_position
	uint16_t abs_pos	 = program_state->absolute_motor_position;
	uint16_t abs_pos_inv = ~abs_pos;

	eeprom_write_u16(EE_ADDR_ABS_POSITION + 2, abs_pos_inv);
	eeprom_write_u16(EE_ADDR_ABS_POSITION, abs_pos);

	// is_running
	uint8_t run_flag	 = program_state->is_running;
	uint8_t run_flag_inv = (uint8_t) ~run_flag;

	eeprom_write_byte(EE_ADDR_IS_RUNNING + 1, run_flag_inv);
	eeprom_write_byte(EE_ADDR_IS_RUNNING, run_flag);

	// current_pill
	uint8_t current_pill	 = program_state->current_pill;
	uint8_t current_pill_inv = (uint8_t) ~current_pill;

	eeprom_write_byte(EE_ADDR_CURRENT_PILL + 1, current_pill_inv);
	eeprom_write_byte(EE_ADDR_CURRENT_PILL, current_pill);
}
