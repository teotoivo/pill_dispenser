#ifndef LOGING_H
#define LOGING_H
#include <stddef.h>
#include <stdint.h>

#define MAX_LOG_ENTRIES 32
#define LOG_ENTRY_SIZE 64

#define LOG(fmt, ...)														  \
  do {																		 \
	/* Create a temporary buffer for the formatted log entry */				\
	char log_buffer[128];													  \
																			   \
	/* Include timestamp in the formatted string */							\
	int len = snprintf(log_buffer, sizeof(log_buffer), "%lld s: " fmt,		 \
					   (uint64_t)(time_us_64() / 1000000), ##__VA_ARGS__);	 \
																			   \
	/* Ensure the string fits in the buffer */								 \
	if ((uint32_t)len >= sizeof(log_buffer)) {								 \
	  log_buffer[sizeof(log_buffer) - 1] =									 \
		  '\0'; /* Null-terminate if overflow */							   \
	}																		  \
																			   \
	printf("%s\n", log_buffer);												\
																			   \
	write_log_entry(log_buffer);											   \
  } while (0)

uint16_t crc16(const uint8_t *data_p, size_t length);
void write_log_entry(const char *log_entry);
void erase_log();
void read_log();

#endif // !loging
