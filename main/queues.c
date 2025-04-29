#include "queues.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "queues.h"
#include "uart_task.h"
#include "imu_task.h"

QueueHandle_t xQueuePos;

void queues_init(void) {
    xQueuePos = xQueueCreate(32, sizeof(mouse_event_t));
    if (xQueuePos == NULL) {
        printf("Erro ao criar fila xQueuePos!\n");
        while (true);
    }
}
