#include "pico/stdlib.h"
#include <pico/util/queue.h>
#include <stdio.h>

#include "hardware_definitions.h"
#include "utils.h"
#include "initial.h"

const uint8_t LEDS[LED_COUNT] = { LED_D1, LED_D2, LED_D3 };
const uint8_t BUTTONS[BUTTON_COUNT] = { SW_0, SW_1, SW_2 };

static queue_t button_events;

void init_leds(size_t led_count)
{
    for (size_t led_i = 0; led_i < led_count; led_i++) {
        init_led_pin(LEDS[led_i]);  
    }
}

void init_buttons(size_t button_count)
{
    for (size_t button_i = 0; button_i < button_count; button_i++) {
        init_button_pin(BUTTONS[button_i]);  
    }
}

static void toggle_leds(size_t led_count, bool state)
{
    for (size_t led_i = 0; led_i < led_count; led_i++) {
        gpio_put(LEDS[led_i], state);
    }
}

void wait_for_calibration()
{
    const uint16_t blinking_interval_ms = 250;

    uint8_t event;
    queue_init(&button_events, sizeof(uint8_t), 20);

    while (true)
    {
        if (queue_try_remove(&button_events, &event)) {
            for (size_t led_i = 0; led_i < LED_COUNT; led_i++) {
                gpio_put(LEDS[led_i], false);
            }

            return;
        }

        toggle_leds(LED_COUNT, true);
        sleep_ms(blinking_interval_ms);
        toggle_leds(LED_COUNT, false);
        sleep_ms(blinking_interval_ms);
    }
}

void wait_for_start()
{
    uint8_t event;
    queue_init(&button_events, sizeof(uint8_t), 20);

    while (queue_try_remove(&button_events, &event));
    toggle_leds(LED_COUNT, true);

    while (!queue_try_remove(&button_events, &event));
    toggle_leds(LED_COUNT, false);
}

void button_interrupt_callback(uint32_t event_mask)
{
    if (event_mask & GPIO_IRQ_EDGE_RISE) {
        uint8_t event = 1;
        queue_try_add(&button_events, &event);
    }
}

