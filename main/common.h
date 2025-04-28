#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <FreeRTOS.h>
#include <queue.h>

// Estrutura para eventos de movimento ou click
typedef struct {
    uint8_t axis;   // 0 = eixo X, 1 = eixo Y, 2 = click, 3 = movimento jogador (joystick analógico)
    int16_t value;
} mouse_event_t;

// Estrutura para eventos dos botões
typedef struct {
    uint8_t button_id;
    uint8_t pressed; // 1 pressionado, 0 solto
} button_event_t;

// Filas globais (apenas DECLARADAS aqui)
extern QueueHandle_t xQueueIMU;
extern QueueHandle_t xQueueJoystick;
extern QueueHandle_t xQueueButtons;
extern QueueHandle_t xQueueUART;
extern QueueHandle_t xQueueFeedback;

#endif
