#include "dummybme280_task.h"

sensor::BME280::Data tasks::DummyBME280::MakeData_() {
    sensor::BME280::Data data;
    data.humidity = 68;
    data.temperature = 25;
    data.pressure = 10220;
    return data;
}

tasks::DummyBME280::DummyBME280()
    : rtos::Task("dummy", configMINIMAL_STACK_SIZE, tskIDLE_PRIORITY) {
}

void tasks::DummyBME280::RegisterListeners(DataObserver<sensor::BME280::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

void tasks::DummyBME280::Run() {
    while (true) {
        sensor::BME280::Data data = MakeData_();
        data_provider_.NotifyListeners(data);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}