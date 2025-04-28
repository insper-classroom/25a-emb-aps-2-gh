#include "uart_task.h"
#include "queues.h"

#include "pico/stdlib.h"
#include <stdio.h>

void uart_task(void *p) {
    mouse_event_t event;
    while (1) {
        if (xQueueReceive(xQueuePos, &event, portMAX_DELAY)) {
            int16_t valor = event.value;

            uint8_t axis = event.axis;
            uint8_t val_0 = (uint8_t)(valor & 0xFF);
            uint8_t val_1 = (uint8_t)((valor >> 8) & 0xFF);
            uint8_t eop = 0xFF;

            // Enviar UART no formato: EOP - AXIS - VAL0 - VAL1
            putchar_raw(eop);
            putchar_raw(axis);
            putchar_raw(val_0);
            putchar_raw(val_1);

            printf("Axis: %d | Value: %d\n", axis, valor);
        }
    }
}
