#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queues.h"
#include "stdbool.h"

#define DEADZONE 80

static int filter_adc(int raw) {
    int centered = raw - 2047;
    int scaled = (255 * centered) / 2047;
    return scaled;
}

void analog_task(void *p) {
    uint adc_channel = *(uint *)p;
    int buffer[5] = {0}, index = 0, count = 0, sum = 0;
    uint gpio = adc_channel == 0 ? 26 : 27;

    adc_gpio_init(gpio);
    adc_select_input(adc_channel);

    bool was_positive = false;
    bool was_negative = false;

    while (1) {
        int leitura = adc_read();

        if (count >= 5) sum -= buffer[index];
        buffer[index] = leitura;
        sum += leitura;
        index = (index + 1) % 5;
        if (count < 5) count++;

        int media = sum / count;
        int valor = filter_adc(media);

        int axis_positive = (adc_channel == 0) ? 6 : 8;  // D ou S
        int axis_negative = (adc_channel == 0) ? 7 : 9;  // A ou W

        if (valor > DEADZONE) {
            if (!was_positive) {
                mouse_event_t event = { .axis = axis_positive, .value = 1 };
                xQueueSend(xQueuePos, &event, 0);
                was_positive = true;
            }
            if (was_negative) {
                mouse_event_t event = { .axis = axis_negative, .value = 0 };
                xQueueSend(xQueuePos, &event, 0);
                was_negative = false;
            }
        } else if (valor < -DEADZONE) {
            if (!was_negative) {
                mouse_event_t event = { .axis = axis_negative, .value = 1 };
                xQueueSend(xQueuePos, &event, 0);
                was_negative = true;
            }
            if (was_positive) {
                mouse_event_t event = { .axis = axis_positive, .value = 0 };
                xQueueSend(xQueuePos, &event, 0);
                was_positive = false;
            }
        } else {
            if (was_positive) {
                mouse_event_t event = { .axis = axis_positive, .value = 0 };
                xQueueSend(xQueuePos, &event, 0);
                was_positive = false;
            }
            if (was_negative) {
                mouse_event_t event = { .axis = axis_negative, .value = 0 };
                xQueueSend(xQueuePos, &event, 0);
                was_negative = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // reduzido para melhorar a resposta
    }
}

void analog_task_init(void) {
    adc_init();
    adc_gpio_init(26); // ADC0 = eixo X
    adc_gpio_init(27); // ADC1 = eixo Y
}
