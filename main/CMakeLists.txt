add_executable(pico_emb
        main.c
        uart_task.c
        queues.c
        imu_task.c
        button_task.c
        analog_task.c
)

set_target_properties(pico_emb PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_link_libraries(pico_emb pico_stdlib freertos hardware_i2c hardware_adc Fusion)
pico_add_extra_outputs(pico_emb)
