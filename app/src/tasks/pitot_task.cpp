#include "pitot_task.h"
#include <cstdio>
#include "adc.h"
#include "semihosting_stream.h"
#include "stm32f4xx_hal_adc.h"
#include "task_configuration.h"

uint16_t ReadADC(ADC_HandleTypeDef* hadc, uint32_t channel) {
    // Configure the channel
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;                        // ADC_CHANNEL_0, ADC_CHANNEL_1, etc.
    sConfig.Rank = 1;                                 // First and only conversion
    sConfig.SamplingTime = ADC_SAMPLETIME_112CYCLES;  // Longest sampling time for stability

    // Apply configuration
    HAL_ADC_ConfigChannel(hadc, &sConfig);

    // Start ADC conversion
    HAL_ADC_Start(hadc);

    // Wait for conversion to complete
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);

    // Read the converted value
    uint16_t value = HAL_ADC_GetValue(hadc);

    // Stop ADC conversion
    HAL_ADC_Stop(hadc);

    return value;
}

namespace tasks {
PitotTask::PitotTask()
    : rtos::Task("Pitot", SENSOR_TASK_STACK, static_cast<UBaseType_t>(Priority::SENSOR)) {
}

void PitotTask::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(PITOT_FREQUENCY.count());

    PitotData data;
    while (true) {
        data.channel1 = ReadADC(&hadc1, ADC_CHANNEL_8);
        data.channel2 = ReadADC(&hadc1, ADC_CHANNEL_9);

        data_provider_.NotifyListeners(data);

        // mcu::semi << data.channel1 << ";" << data.channel2 << "\n";
        xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

void PitotTask::RegisterListener(DataObserver<PitotData>& observer) {
    data_provider_.Subscribe(&observer);
}
}  // namespace tasks
