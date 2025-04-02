#include "i2c_threadsafe.h"
#include <cstddef>
#include <cstdint>
#include "FreeRTOS.h"
#include "i2c_device.h"
#include "portmacro.h"
#include "projdefs.h"
#include "semphr.h"
#include "stm32f4xx_hal_i2c.h"

namespace communication {
SemaphoreHandle_t bus_mutex{xSemaphoreCreateMutex()};

I2CDeviceThreadsafe::I2CDeviceThreadsafe(I2C_HandleTypeDef& i2c_handle, uint8_t device_address)
    : I2CDevice(i2c_handle, device_address) {}

bool I2CDeviceThreadsafe::Read(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout) {
    if (xSemaphoreTake(bus_mutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }
    auto result = I2CDevice::Read(register_address, buffer, length, timeout);
    xSemaphoreGive(bus_mutex);
    return result;
}

bool I2CDeviceThreadsafe::Write(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout) {
    if (xSemaphoreTake(bus_mutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }
    auto result = I2CDevice::Write(register_address, buffer, length, timeout);
    xSemaphoreGive(bus_mutex);
    return result;
}

bool I2CDeviceThreadsafe::Read(uint8_t* buffer, size_t length, uint32_t timeout) {
    if (xSemaphoreTake(bus_mutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }
    auto result = I2CDevice::Read(buffer, length, timeout);
    xSemaphoreGive(bus_mutex);
    return result;
}

bool I2CDeviceThreadsafe::Write(const uint8_t* buffer, size_t length, uint32_t timeout) {
    if (xSemaphoreTake(bus_mutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }
    auto result = I2CDevice::Write(buffer, length, timeout);
    xSemaphoreGive(bus_mutex);
    return result;
}
}  // namespace communication
