#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

/*
	Create "include/lora_api_key.h" file and add your api key there:
	#ifndef LORA_API_KEY_H
	#define LORA_API_KEY_H

	#define LORA_API_KEY "your_lora_api_key"

	#endif
*/
#include "hardware_definitions.h"
#include "lora.h"
#include "lora_api_key.h"
#include "utils.h"

typedef struct LoraCommand
{
	char *command;
	char *expected_result;
	uint32_t timeout_ms;
} LoraCommand;

static void send_command(const char *command, char *result_buffer,
						 uint32_t timeout_ms)
{
	uart_write_blocking(uart1, (uint8_t *) command, strlen(command));
	uart_write_blocking(uart1, (uint8_t *) "\r\n", 2);

	size_t result_position = 0;

	while (uart_is_readable_within_us(uart1, timeout_ms * 1000))
	{
		char chr = uart_getc(uart1);
		if (result_position < MAX_RESULT_SIZE - 1)
		{
			result_buffer[result_position++] = chr;
		}
		else
		{
			break;
		}
	}
	result_buffer[result_position] = '\0';
}

bool connect_to_lora_module()
{
	printf("Starting lora connection...\n");

	uart_init(uart1, BAUD_RATE);

	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

	gpio_pull_up(UART_RX_PIN);

	char *at_command	= str_concat("AT", "");
	char *mode_command	= str_concat("AT+MODE=", LORA_MODE);
	char *key_command	= str_concat("AT+KEY=APPKEY,", LORA_API_KEY);
	char *class_command = str_concat("AT+CLASS=", LORA_CLASS);
	char *port_command	= str_concat("AT+PORT=", LORA_PORT);
	char *join_command	= str_concat("AT+JOIN", "");

	LoraCommand connectiond_cmds[CONNECTION_COMMAND_COUNT] = {
		{at_command, "+AT: OK", 500},
		{mode_command, "+MODE", 500},
		{key_command, "+KEY: APPKEY", 500},
		{class_command, "+CLASS:", 500},
		{port_command, "+PORT:", 500},
		{join_command, "+JOIN: NetID", 20000}};

	bool connection_succeeded = false;

	uint8_t join_command_attempts = 0;
	uint8_t curr_cmd_i			  = 0;

	while (curr_cmd_i < CONNECTION_COMMAND_COUNT)
	{
		if (!connectiond_cmds[curr_cmd_i].command)
		{
			connection_succeeded = false;
			break;
		}

		char result[MAX_RESULT_SIZE];

		printf("Sending: %s\n", connectiond_cmds[curr_cmd_i].command);

		send_command(connectiond_cmds[curr_cmd_i].command, result,
					 connectiond_cmds[curr_cmd_i].timeout_ms);

		if (strstr(result, connectiond_cmds[curr_cmd_i].expected_result) !=
			NULL)
		{
			printf("%s", result);
			printf("Success: %s\n\n", connectiond_cmds[curr_cmd_i].command);
			curr_cmd_i++;
			connection_succeeded = true;
		}
		else
		{
			connection_succeeded = false;

			if (curr_cmd_i == 5 && join_command_attempts < 2)
			{
				join_command_attempts++;
				curr_cmd_i = 5;
				printf("Join command failed attempt %d. Retrying...\n",
					   join_command_attempts);
			}
			else
			{
				printf("%s", result);
				printf("Failed: %s\n\n", connectiond_cmds[curr_cmd_i].command);
				break;
			}
		}
	}

	for (uint8_t i = 0; i < CONNECTION_COMMAND_COUNT; i++)
	{
		free(connectiond_cmds[i].command);
	}

	return connection_succeeded;
}

bool send_message(bool connected, const char *fmt, ...)
{
	if (!connected)
	{
		return false;
	}
	if (!fmt)
	{
		printf("Message sent to LoRa cant be empty\n");
		return false;
	}

	char message[512];
	va_list args;
	va_start(args, fmt);
	vsnprintf(message, sizeof(message), fmt, args);
	va_end(args);

	if (strlen(message) == 0) {
		printf("Error formating message\n");
		return false;
	}

	size_t message_len		  = strlen(message);
	size_t quoted_message_len = message_len + 3;

	char *quoted_message = (char *) malloc(quoted_message_len + 1);
	if (!quoted_message)
	{
		printf("Internal error\n");
		return false;
	}

	quoted_message[0] = '"';
	memcpy(quoted_message + 1, message, message_len);
	quoted_message[message_len + 1] = '"';
	quoted_message[message_len + 2] = '\0';

	char *msg_command = str_concat("AT+MSG=", quoted_message);
	free(quoted_message);

	if (!msg_command)
	{
		printf("Error creating msg command.\n");
		return false;
	}

	printf("Sending: %s\n", msg_command);

	char result[MAX_RESULT_SIZE];
	send_command(msg_command, result, 10000 /* wait response for 10 seconds */);
	printf("%s\n", result);

	return strstr(result, "+MSG: Done") != NULL;
}
