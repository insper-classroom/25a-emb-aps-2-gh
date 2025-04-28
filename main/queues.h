#ifndef QUEUES_H
#define QUEUES_H

#include <FreeRTOS.h>
#include <queue.h>

// Estrutura para eventos de movimento/click
typedef struct {
    uint8_t axis;    // 0: eixo X, 1: eixo Y, 2: click
    int16_t value;   // valor do movimento ou clique
} mouse_event_t;

extern QueueHandle_t xQueuePos;

void queues_init(void);

#endif // QUEUES_H
