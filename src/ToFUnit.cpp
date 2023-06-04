#include "ToFUnit.hpp"

ToFUnit::ToFUnit(TwoWire& wire, uint8_t sda, uint8_t scl, uint16_t timeout)
    : _sda(sda), _scl(scl), _timeout(timeout), _sensor(), _wire(wire) {
}

ToFUnit::~ToFUnit(void) {
}

const char* ToFUnit::getName(void) const {
    return "ToF Unit";
}

bool ToFUnit::begin(void) {
    this->_wire.begin(this->_sda, this->_scl);
    this->_sensor.setBus(&this->_wire);
    this->_sensor.setAddress(I2C_ADDRESS);
    this->_sensor.setTimeout(this->_timeout);
    if (this->_sensor.init() == false) {
        ESP_LOGE(getName(), "Failed to detect and initialize ToF Unit");
        return false;
    }
    this->_sensor.setMeasurementTimingBudget(200000);  // 高精度
    this->_sensor.startContinuous();
    return true;
}

bool ToFUnit::getDistance(distance_unit_t& distance) {
    distance_unit_t d = this->_sensor.readRangeContinuousMillimeters();
    ESP_LOGD("ToFUnit", "Raw Distance: %dmm", d);
    if (this->_sensor.timeoutOccurred()) {
        ESP_LOGW(getName(), "Timeout");
        return false;
    }
    if (OUT_OF_RANGE_MIN <= d && d <= OUT_OF_RANGE_MAX) {
        ESP_LOGW(getName(), "Out of Range");
        return false;
    }
    distance = d;
    return true;
}

distance_unit_t ToFUnit::getMinDistance(void) const {
    return MIN_DISTANCE_MM;
}

distance_unit_t ToFUnit::getMaxDistance(void) const {
    return MAX_DISTANCE_MM;
}

double ToFUnit::getAccuracy(void) const {
    return ACCURACY;
}
