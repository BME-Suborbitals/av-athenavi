#include "spi_threadsafe_dma.h"
#include <cstddef>
#include <cstdint>
#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "spi_device.h"
#include "stm32f4xx_hal_spi.h"

extern "C" {
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {
    SemaphoreHandle_t dma_semaphore = communication::SPIThreadsafeDMA::GetDMASemaphore(hspi);
    auto higher_priority_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(dma_semaphore, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);  // NOLINT
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {
    SemaphoreHandle_t dma_semaphore = communication::SPIThreadsafeDMA::GetDMASemaphore(hspi);
    auto higher_priority_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(dma_semaphore, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);  // NOLINT
}
}

namespace communication {
std::unordered_map<SPIHandle*, SemaphoreHandle_t> SPIThreadsafeDMA::dma_semaphore_map;  // NOLINT
std::unordered_map<SPIHandle*, SemaphoreHandle_t> SPIThreadsafeDMA::bus_mutex_map;      //NOLINT

SPIThreadsafeDMA::SPIThreadsafeDMA(SPIHandle* spi_handle, GPIOPort port, GPIOPin pin)
    : SPIDevice(spi_handle, port, pin) {
}

SemaphoreHandle_t& SPIThreadsafeDMA::GetBusMutex(SPIHandle* spi_handle) {
    if (!bus_mutex_map.contains(spi_handle)) {
        bus_mutex_map.insert({spi_handle, xSemaphoreCreateMutex()});
    }
    return bus_mutex_map.at(spi_handle);
}

SemaphoreHandle_t& SPIThreadsafeDMA::GetDMASemaphore(SPIHandle* spi_handle) {
    if (!dma_semaphore_map.contains(spi_handle)) {
        dma_semaphore_map.insert({spi_handle, xSemaphoreCreateBinary()});
    }
    return dma_semaphore_map.at(spi_handle);
}

void SPIThreadsafeDMA::BeginTransaction() {
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        (void)GetDMASemaphore(GetSPIHandle());
        if (xSemaphoreTake(GetBusMutex(GetSPIHandle()), portMAX_DELAY) != pdTRUE) {
            return;
        }
    }
    SPIDevice::BeginTransaction();
}

void SPIThreadsafeDMA::EndTransaction() {
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        xSemaphoreGive(GetBusMutex(GetSPIHandle()));
    }
    SPIDevice::EndTransaction();
}

void SPIThreadsafeDMA::Read(uint8_t* buffer, size_t length) {
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        SPIDevice::Read(buffer, length);
        return;
    }
    HAL_SPI_Receive_DMA(GetSPIHandle(), buffer, length);
    xSemaphoreTake(GetDMASemaphore(GetSPIHandle()), portMAX_DELAY);
}

void SPIThreadsafeDMA::Write(const uint8_t* buffer, size_t length) {
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        SPIDevice::Write(buffer, length);
        return;
    }
    HAL_SPI_Transmit_DMA(GetSPIHandle(), buffer, length);
    xSemaphoreTake(GetDMASemaphore(GetSPIHandle()), portMAX_DELAY);
}
}  // namespace communication
