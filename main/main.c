#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "queues.h"
#include "uart_task.h"
#include "imu_task.h"
#include "button_task.h"
#include "analog_task.h"

int main() {
    stdio_init_all();

    queues_init(); // Inicializa todas as filas
    button_task_init();
    analog_task_init(); // Inicializa a tarefa de leitura analógica

    xTaskCreate(uart_task, "UART Task", 4096, NULL, 1, NULL);
    xTaskCreate(imu_task, "IMU Task", 4096, NULL, 1, NULL);

    static uint adc_x = 0;
    static uint adc_y = 1;
    
    xTaskCreate(analog_task, "Analog X Task", 2048, &adc_x, 1, NULL);
    xTaskCreate(analog_task, "Analog Y Task", 2048, &adc_y, 1, NULL);

    vTaskStartScheduler();

    while (true);
}
