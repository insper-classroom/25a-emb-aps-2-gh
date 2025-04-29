#include "pico/stdlib.h"
#include "queues.h"
#include <stdio.h>

// Estrutura dos dados:
// eop (0xFF) → axis (uint8_t) → val_0 (LSB) → val_1 (MSB)

void uart_task(void *p) {
    mouse_event_t event;

    while (1) {
        if (xQueueReceive(xQueuePos, &event, portMAX_DELAY)) {
            int16_t valor = event.value;
            uint8_t axis = event.axis;

            uint8_t val_0 = (uint8_t)(valor & 0xFF);
            uint8_t val_1 = (uint8_t)((valor >> 8) & 0xFF);
            uint8_t eop = 0xFF;

            putchar_raw(eop);
            putchar_raw(axis);
            putchar_raw(val_0);
            putchar_raw(val_1);

            // Debug (pode comentar para reduzir ruído na serial)
            switch (axis) {
                case 2: printf("[Click]\n"); break;
                case 3: printf("[Reload]\n"); break;
                case 4: printf("[Jump]\n"); break;
                case 5: printf("[Switch Weapon]\n"); break;
                default: printf("", axis, valor); break;
            }
        }
    }
}
