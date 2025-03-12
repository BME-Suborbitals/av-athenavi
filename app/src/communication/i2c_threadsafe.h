#ifndef I2C_THREADSAFE_H
#define I2C_THREADSAFE_H

#include <cstddef>
#include <cstdint>
#include "FreeRTOS.h"
#include "i2c_device.h"
#include "semphr.h"
#include "stm32f4xx_hal_i2c.h"

namespace communication {

/**
 * @brief Threadsafe wrapper for I2CDevice
 * 
 * This class provides a thread-safe implementation of the I2CDevice interface by
 * using a mutex to ensure exclusive access to the I2C bus during communication.
 * This prevents concurrent access from multiple tasks which could corrupt data
 * or cause bus contention.
 */
class I2CDeviceThreadsafe : public I2CDevice {
  private:
    // TODO: Separate mutexes for different buses
    static SemaphoreHandle_t bus_mutex;

  public:
    /**
     * @brief Constructs an I2CDeviceThreadsafe instance
     * @param i2c_handle Reference to the HAL I2C handle
     * @param device_address The 7-bit I2C device address
     */
    I2CDeviceThreadsafe(I2C_HandleTypeDef& i2c_handle, uint8_t device_address);

    /**
     * @brief Reads data from a specific register in thread-safe manner
     * @param register_address The register address to read from
     * @param buffer Pointer to buffer where read data will be stored
     * @param length Number of bytes to read
     * @param timeout Timeout in milliseconds
     * @return true if read was successful, false otherwise
     */
    bool Read(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT) override;

    /**
     * @brief Writes data to a specific register in thread-safe manner
     * @param register_address The register address to write to
     * @param buffer Pointer to buffer containing data to write
     * @param length Number of bytes to write
     * @param timeout Timeout in milliseconds
     * @return true if write was successful, false otherwise
     */
    bool Write(uint8_t register_address, uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT) override;

    /**
     * @brief Reads data directly from the device in thread-safe manner
     * @param buffer Pointer to buffer where read data will be stored
     * @param length Number of bytes to read
     * @param timeout Timeout in milliseconds
     * @return true if read was successful, false otherwise
     */
    bool Read(uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT) override;

    /**
     * @brief Writes data directly to the device in thread-safe manner
     * @param buffer Pointer to buffer containing data to write
     * @param length Number of bytes to write
     * @param timeout Timeout in milliseconds
     * @return true if write was successful, false otherwise
     */
    bool Write(const uint8_t* buffer, size_t length, uint32_t timeout = MAX_TIMEOUT) override;
};
}  // namespace communication

#endif  //I2C_THREADSAFE_H