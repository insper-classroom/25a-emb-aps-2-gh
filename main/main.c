#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "queues.h"
#include "uart_task.h"
#include "imu_task.h"

int main() {
    stdio_init_all();

    queues_init(); // Inicializa todas as filas

    xTaskCreate(uart_task, "UART Task", 4096, NULL, 1, NULL);
    xTaskCreate(imu_task, "IMU Task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true);
}
