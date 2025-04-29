#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queues.h"
#include "hardware/adc.h"

#define ANALOG_X_CHANNEL 0  // GPIO 26
#define ANALOG_Y_CHANNEL 1  // GPIO 27

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

    int last_direction = 0;

    while (1) {
        int leitura = adc_read();

        if (count >= 5) sum -= buffer[index];
        buffer[index] = leitura;
        sum += leitura;
        index = (index + 1) % 5;
        if (count < 5) count++;

        int media = sum / count;
        int valor = filter_adc(media);

        int dir = 0;
        if (valor > 80) dir = 1;
        else if (valor < -80) dir = -1;

        if (dir != last_direction) {
            mouse_event_t event;

            if (last_direction != 0) {
                event.axis = (adc_channel == 0)
                    ? (last_direction > 0 ? 6 : 7)
                    : (last_direction > 0 ? 8 : 9);
                event.value = 0;
                xQueueSend(xQueuePos, &event, 0);
            }

            if (dir != 0) {
                event.axis = (adc_channel == 0)
                    ? (dir > 0 ? 6 : 7)
                    : (dir > 0 ? 8 : 9);
                event.value = 1;
                xQueueSend(xQueuePos, &event, 0);
            }

            last_direction = dir;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void analog_task_init(void) {
    adc_init();
    adc_gpio_init(26); // X
    adc_gpio_init(27); // Y
}
