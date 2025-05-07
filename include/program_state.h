#ifndef PROGRAM_STATE_H
#define PROGRAM_STATE_H
#include <stdint.h>
typedef struct ProgramState
{
	uint16_t steps_per_rev;
	uint16_t absolute_motor_position;
	uint8_t is_running;	 // 0 for no 1 for yes
	uint8_t current_pill;
	bool is_lora_connected;
} ProgramState;

/* --------------------------------------------------------------------
 * EEPROM layout (highest addresses reserve space for ProgramState)
 *
 * 0x7FFF  ┐
 * 0x7FFE  │  steps_per_rev      (low, high)
 * 0x7FFD  │  ~steps_per_rev     (low, high)
 * 0x7FFC  ┘
 * 0x7FFB  ┐
 * 0x7FFA  │  absolute_position  (low, high)
 * 0x7FF9  │  ~absolute_position (low, high)
 * 0x7FF8  ┘
 * 0x7FF7  is_running            (1 byte)
 * 0x7FF6  ~is_running           (1 byte)
 * 0x7FF5  current_pill          (1 byte)
 * 0x7FF4  ~current_pill         (1 byte)
 * ------------------------------------------------------------------ */

#define EE_ADDR_STEPS_PER_REV (EEPROM_MAX_ADDRESS - 3) /* 0x7FFC */
#define EE_ADDR_ABS_POSITION (EEPROM_MAX_ADDRESS - 7)  /* 0x7FF8 */
#define EE_ADDR_IS_RUNNING (EEPROM_MAX_ADDRESS - 9)	   /* 0x7FF6 */
#define EE_ADDR_CURRENT_PILL (EEPROM_MAX_ADDRESS - 11) /* 0x7FF4 */

#define PILL_SLOTS 8  // first one is always empty

void load_program_state(ProgramState* program_state);
void write_program_state(ProgramState* program_state);

#endif	// !PROGRAM_STATE_H
