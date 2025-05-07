#include <stdbool.h>

#ifndef LORA_H
#define LORA_H

#define CONNECTION_COMMAND_COUNT 6
#define MAX_RESULT_SIZE 255

#define LORA_MODE "LWOTAA"
#define LORA_CLASS "A"
#define LORA_PORT "8"

bool connect_to_lora_module();
bool send_message(const char *message, bool connected);

#endif
