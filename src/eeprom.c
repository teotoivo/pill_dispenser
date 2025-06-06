#include <hardware/i2c.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>

#include "hardware_definitions.h"

void eeprom_write_byte(uint16_t memory_address, uint8_t data)
{
	uint8_t buff[3];

	buff[0] = memory_address >> 8;	  // High byte
	buff[1] = memory_address & 0xFF;  // Low byte

	buff[2] = data;

	i2c_write_blocking(EEPROM_PORT, EEPROM_ADDR, buff, 3, false);
	sleep_ms(5);
}

uint8_t eeprom_read_byte(uint16_t memory_address)
{
	uint8_t buff[2];

	buff[0] = memory_address >> 8;	  // High byte
	buff[1] = memory_address & 0xFF;  // Low byte

	i2c_write_blocking(EEPROM_PORT, EEPROM_ADDR, buff, 2,
					   true);  // send address

	uint8_t result;
	i2c_read_blocking(EEPROM_PORT, EEPROM_ADDR, &result, 1, false);

	return result;
}

uint16_t eeprom_read_u16(uint16_t addr)
{
	uint16_t lo = eeprom_read_byte(addr);
	uint16_t hi = eeprom_read_byte(addr + 1);
	return (uint16_t) ((hi << 8) | lo);
}
void eeprom_write_u16(uint16_t addr, uint16_t value)
{
	eeprom_write_byte(addr, (uint8_t) (value & 0xFF));
	eeprom_write_byte(addr + 1, (uint8_t) ((value >> 8) & 0xFF));
}
