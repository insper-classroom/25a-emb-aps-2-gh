// button_task.c
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "queues.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// Definições dos pinos dos botões
#define BUTTON_SHOOT 15    // Gatilho
#define BUTTON_RELOAD 13   // Recarregar
#define BUTTON_JUMP 14     // Pulo
#define BUTTON_SWITCH 12   // Trocar arma
#define VIBRACAL_PIN   7   // Pino do vibrador

volatile bool should_vibrate = false;

void gpio_callback(uint gpio, uint32_t events) {
    mouse_event_t event;

    if (events & GPIO_IRQ_EDGE_FALL) {  // Detecta botão pressionado (bordo de descida)
        switch (gpio) {
            case BUTTON_SHOOT:
                event.axis = 2;  // Eixo para "Mouse Click" (já usado no IMU também)
                event.value = 1;
                xQueueSendFromISR(xQueuePos, &event, NULL);
                should_vibrate = true;
                return;
            case BUTTON_RELOAD:
                event.axis = 3;  // Eixo especial para "Reload"
                event.value = 1;
                break;
            case BUTTON_JUMP:
                event.axis = 4;  // Eixo especial para "Jump"
                event.value = 1;
                break;
            case BUTTON_SWITCH:
                event.axis = 5;  // Eixo especial para "Switch Weapon"
                event.value = 1;
                break;
            default:
                return;
        }

        xQueueSendFromISR(xQueuePos, &event, NULL);
    }
}

void vibration_task(void *p) {
    while (1) {
        if (should_vibrate) {
            gpio_put(VIBRACAL_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(300));
            gpio_put(VIBRACAL_PIN, 0);
            should_vibrate = false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void button_task_init(void) {
    gpio_init(BUTTON_SHOOT);
    gpio_set_dir(BUTTON_SHOOT, GPIO_IN);
    gpio_pull_up(BUTTON_SHOOT);

    gpio_init(BUTTON_RELOAD);
    gpio_set_dir(BUTTON_RELOAD, GPIO_IN);
    gpio_pull_up(BUTTON_RELOAD);

    gpio_init(BUTTON_JUMP);
    gpio_set_dir(BUTTON_JUMP, GPIO_IN);
    gpio_pull_up(BUTTON_JUMP);

    gpio_init(BUTTON_SWITCH);
    gpio_set_dir(BUTTON_SWITCH, GPIO_IN);
    gpio_pull_up(BUTTON_SWITCH);

    gpio_init(VIBRACAL_PIN);
    gpio_set_dir(VIBRACAL_PIN, GPIO_OUT);
    gpio_put(VIBRACAL_PIN, 0);  // Desligado inicialmente

    gpio_set_irq_enabled_with_callback(BUTTON_SHOOT, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    gpio_set_irq_enabled(BUTTON_RELOAD, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_JUMP, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_SWITCH, GPIO_IRQ_EDGE_FALL, true);

    xTaskCreate(vibration_task, "Vibration Task", 1024, NULL, 1, NULL);
}
