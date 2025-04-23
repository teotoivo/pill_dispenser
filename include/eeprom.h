#ifndef EEPROM_H
#define EEPROM_H
#include <stdint.h>

#define EEPROM_MAX_ADDRESS 0x7FFF

#define EEPROM_BAUD_RATE 100000	 // 100KHz

uint8_t eeprom_read_byte(uint16_t memory_address);
void eeprom_write_byte(uint16_t memory_address, uint8_t data);

uint16_t eeprom_read_u16(uint16_t addr);
void eeprom_write_u16(uint16_t addr, uint16_t value);

#endif	// !EEPROM_H
