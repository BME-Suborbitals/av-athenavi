// #include "uart_device_dma.h"
// #include "FreeRTOS.h"
// #include "semphr.h"
// #include "stm32f4xx_hal_uart.h"

// uint8_t read_buffer[1024];
// uint16_t index{0};
// SemaphoreHandle_t line_read = xSemaphoreCreateBinary();

// extern "C" {
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
//     if (read_buffer[index] != '\n') {
//         HAL_UART_Receive_DMA(huart, &read_buffer[++index], 1);
//         return;
//     }

//     auto higher_priority_task_woken = pdFALSE;
//     xSemaphoreGiveFromISR(line_read, &higher_priority_task_woken);
//     portYIELD_FROM_ISR(higher_priority_task_woken);  // NOLINT
// }
// }

// namespace communication {

// UARTDevice::UARTDevice(UART_HandleTypeDef* huart)
//     : huart_(huart) {}

// bool UARTDevice::Read(uint8_t* buffer, size_t length, uint32_t timeout) {
//     if (HAL_UART_GetState(huart_) != HAL_UART_STATE_READY) {
//         return false;
//     }
//     HAL_StatusTypeDef status = HAL_UART_Receive(huart_, buffer, length, timeout);
//     if (status != HAL_OK && status != HAL_TIMEOUT) {
//         return false;
//     }
//     uint16_t bytes_received = length - __HAL_UART_GET_FLAG(huart_, UART_FLAG_RXNE);
//     if (bytes_received == 0)
//         return false;

//     return true;
// }

// bool UARTDevice::Write(const uint8_t* buffer, size_t length, uint32_t timeout) {
//     return HAL_UART_Transmit(huart_, buffer, length, timeout);
// }

// bool UARTDevice::Readline(std::string& buffer) {
//     // char input = ' ';
//     // while (input != '\n') {  //COULD ENTER INFINITE LOOP
//     //     if (!Read((uint8_t*)&input, 1, 100))
//     //         return false;
//     //     buffer.push_back(input);
//     // }
//     index = 0;
//     HAL_UART_Receive_DMA(huart_, read_buffer, 1);
//     xSemaphoreTake(line_read, portMAX_DELAY);
//     read_buffer[++index] = '\0';
//     buffer.assign(reinterpret_cast<const char*>(read_buffer), index);
//     return true;
// }

// bool UARTDevice::Read(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout) {
//     return false;
// }

// bool UARTDevice::Write(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout) {
//     return false;
// }

// }  // namespace communication