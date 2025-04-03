#ifndef UART_DEVICE_H
#define UART_DEVICE_H

#include <string>
#include "device.h"
#include "usart.h"

namespace communication {

class UARTDevice : public Device {
  public:
    UARTDevice(UART_HandleTypeDef* huart);

    bool Read(uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT);
    bool Write(const uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT);

    bool Read(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT);
    bool Write(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT);

    bool Readline(std::string& buffer);

  private:
    UART_HandleTypeDef* huart_;
};
}  // namespace communication

#endif