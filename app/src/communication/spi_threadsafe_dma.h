#ifndef SPI_THREADSAFE_DMA_H
#define SPI_THREADSAFE_DMA_H

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include "FreeRTOS.h"
#include "semphr.h"
#include "spi_device.h"

namespace communication {

class SPIThreadsafeDMA : public SPIDevice {
  private:
    static std::unordered_map<SPIHandle*, SemaphoreHandle_t> bus_mutex_map;
    static std::unordered_map<SPIHandle*, SemaphoreHandle_t> dma_semaphore_map;

  public:
    SPIThreadsafeDMA(SPIHandle* spi_handle, GPIOPort port, GPIOPin pin);

    [[nodiscard]] static SemaphoreHandle_t& GetBusMutex(SPIHandle* spi_handle);
    [[nodiscard]] static SemaphoreHandle_t& GetDMASemaphore(SPIHandle* spi_handle);

    void BeginTransaction() override;
    void EndTransaction() override;
    void Read(uint8_t* buffer, size_t length) override;
    void Write(const uint8_t* buffer, size_t length) override;
};
}  // namespace communication

#endif  //SPI_THREADSAFE_DMA_H