#include "imu_task.h"
#include "queues.h"
#include <FreeRTOS.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"
#include "Fusion.h"

#include <stdio.h>
#include <math.h>



#define SAMPLE_PERIOD (0.01f) // 10 ms, equivalente a 100Hz

const int MPU_ADDRESS = 0x68;
const int I2C_SDA_GPIO = 4;
const int I2C_SCL_GPIO = 5;

static void mpu6050_reset() {
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, MPU_ADDRESS, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[6];
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, MPU_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer, 6, false);
    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8) | buffer[i * 2 + 1];
    }

    val = 0x43;
    i2c_write_blocking(i2c_default, MPU_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer, 6, false);
    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8) | buffer[i * 2 + 1];
    }

    val = 0x41;
    i2c_write_blocking(i2c_default, MPU_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer, 2, false);
    *temp = (buffer[0] << 8) | buffer[1];
}

void imu_task(void *p) {
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(I2C_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_GPIO);
    gpio_pull_up(I2C_SCL_GPIO);

    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);
    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;

    while (1) {
        mpu6050_read_raw(acceleration, gyro, &temp);

        FusionVector gyroscope = {
            .axis.x = gyro[0] / 131.0f,
            .axis.y = gyro[1] / 131.0f,
            .axis.z = gyro[2] / 131.0f,
        };

        FusionVector accelerometer = {
            .axis.x = acceleration[0] / 16384.0f,
            .axis.y = acceleration[1] / 16384.0f,
            .axis.z = acceleration[2] / 16384.0f,
        };

        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);

        FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

        // Ajuste de sensitividade
        float roll = euler.angle.roll;
        float pitch = euler.angle.pitch;

        if (fabsf(roll) < 5.0f) roll = 0;
        if (fabsf(pitch) < 5.0f) pitch = 0;

        int16_t move_x = (int16_t)(roll * 5.0f);
        int16_t move_y = (int16_t)(-pitch * 5.0f);

        if (move_x != 0) {
            mouse_event_t event = { .axis = 0, .value = move_x };
            xQueueSend(xQueuePos, &event, 0);
        }

        if (move_y != 0) {
            mouse_event_t event = { .axis = 1, .value = move_y };
            xQueueSend(xQueuePos, &event, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Delay 10ms = 100Hz
    }
}
