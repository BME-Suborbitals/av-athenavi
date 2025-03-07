#ifndef I2C_THREADSAFE_H
#define I2C_THREADSAFE_H

#include <cstddef>
#include <cstdint>
#include "FreeRTOS.h"
#include "i2c_device.h"
#include "semphr.h"
#include "stm32f4xx_hal_i2c.h"

namespace communication {
class I2CDeviceThreadsafe : public I2CDevice {
  private:
    // TODO: Separate mutexes for different buses
    static SemaphoreHandle_t bus_mutex;

  public:
    I2CDeviceThreadsafe(I2C_HandleTypeDef& i2c_handle, uint8_t device_address);
    bool Read(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT) override;
    bool Write(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT) override;
};
}  // namespace communication

#endif  //I2C_THREADSAFE_H