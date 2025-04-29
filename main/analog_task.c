#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queues.h"

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

    while (1) {
        int leitura = adc_read();

        if (count >= 5) sum -= buffer[index];
        buffer[index] = leitura;
        sum += leitura;
        index = (index + 1) % 5;
        if (count < 5) count++;

        int media = sum / count;
        int valor = filter_adc(media);

        if (valor > 30 || valor < -30) {
            mouse_event_t event;
            event.axis = (adc_channel == 0)
                ? (valor > 0 ? 6 : 7)  // X eixo: direita ou esquerda
                : (valor > 0 ? 8 : 9); // Y eixo: baixo ou cima
            event.value = 1;
            xQueueSend(xQueuePos, &event, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void analog_task_init(void) {
    adc_init();
    adc_gpio_init(26); // X
    adc_gpio_init(27); // Y
    xTaskCreate(analog_task, "Analog Task", 2048, NULL, 1, NULL);
}