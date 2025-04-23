#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
typedef struct ProgramState
{
	uint64_t steps_per_rev;
	uint64_t absolute_motor_position;
} ProgramState;

#endif	// !GLOBALS_H
