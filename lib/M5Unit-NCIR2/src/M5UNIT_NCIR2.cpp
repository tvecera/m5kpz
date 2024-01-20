#include "M5UNIT_NCIR2.h"

bool M5UNIT_NCIR2::begin(TwoWire *wire, uint8_t sda, uint8_t scl,
                         uint8_t addr) {
    _wire = wire;
    _addr = addr;
    _sda  = sda;
    _scl  = scl;
#ifdef PICOPAD
	_wire->setSDA(sda);
	_wire->setSCL(scl);
	_wire->setTimeout(100);
	_wire->begin();
#else
	_wire->begin(_sda, _scl);
#endif
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

bool M5UNIT_NCIR2::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                              uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->write(buffer, length);
    if (_wire->endTransmission() == 0) return true;
    return false;
}

bool M5UNIT_NCIR2::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                             uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission();
    if (_wire->requestFrom(addr, length)) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[index++] = _wire->read();
        }
        return true;
    }
    return false;
}

int16_t M5UNIT_NCIR2::getTempValue(void) {
    uint8_t data[4];
    readBytes(_addr, M5UNIT_NCIR2_TEMP_REG, data, 2);
    int16_t value = data[0] | (data[1] << 8);
    return value;
}

int16_t M5UNIT_NCIR2::getSocTempValue(void) {
    uint8_t data[4];
    readBytes(_addr, M5UNIT_NCIR2_SOC_TEMP_REG, data, 2);
    int16_t value = data[0] | (data[1] << 8);
    return value;
}

bool M5UNIT_NCIR2::setEmissivity(float emissivity) {
    if (emissivity < 0.1 || emissivity > 1.0) return 0;

    uint16_t data;
    bool result;
    data = (uint16_t)(65535.0f * emissivity);
    result =
        writeBytes(_addr, M5UNIT_NCIR2_EMISSIVITY_REG, (uint8_t *)&data, 2);
    delay(100);
    return result;
}

float M5UNIT_NCIR2::getEmissivity(void) {
    uint8_t data[4];
    float result;
    uint16_t rx_data;

    readBytes(_addr, M5UNIT_NCIR2_EMISSIVITY_REG, data, 2);
    rx_data = data[0] | (data[1] << 8);
    result  = (float)rx_data / 65535.0f;
    return result;
}

bool M5UNIT_NCIR2::setAlarmTemp(uint8_t index, int16_t temp) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_ALARM_TEMP_REG + index * 2;
    data[0]     = temp & 0xff;
    data[1]     = (temp >> 8) & 0xff;
    return writeBytes(_addr, reg, data, 2);
}

bool M5UNIT_NCIR2::getAlarmTemp(uint8_t index, int16_t *temp) {
    if (index > 1) return 0;

    uint8_t data[4];
    bool result;

    uint8_t reg = M5UNIT_NCIR2_ALARM_TEMP_REG + index * 2;
    result      = readBytes(_addr, reg, data, 2);
    if (result) {
        int16_t value = data[0] | (data[1] << 8);
        *temp         = value;
    }
    return result;
}

bool M5UNIT_NCIR2::setAlarmLED(uint8_t index, uint32_t color) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_ALARM_LED_REG + index * 3;
    data[2]     = color & 0xff;
    data[1]     = (color >> 8) & 0xff;
    data[0]     = (color >> 16) & 0xff;
    writeBytes(_addr, reg, data, 3);
		return true;
}

bool M5UNIT_NCIR2::getAlarmLED(uint8_t index, uint32_t *color) {
    if (index > 1) return 0;

    uint8_t data[4];
    bool result;

    uint8_t reg = M5UNIT_NCIR2_ALARM_LED_REG + index * 3;
    result      = readBytes(_addr, reg, data, 3);
    if (result) {
        uint32_t value = (data[0] << 16) | (data[1] << 8) | data[2];
        *color         = value;
    }
    return result;
}

bool M5UNIT_NCIR2::setAlarmBuzzFreq(uint8_t index, uint16_t freq) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_ALARM_BUZZ_REG + index * 5;
    data[0]     = freq & 0xff;
    data[1]     = (freq >> 8) & 0xff;
    return writeBytes(_addr, reg, data, 2);
}

bool M5UNIT_NCIR2::getAlarmBuzzFreq(uint8_t index, uint16_t *freq) {
    if (index > 1) return 0;

    uint8_t data[4];
    bool result;

    uint8_t reg = M5UNIT_NCIR2_ALARM_BUZZ_REG + index * 5;
    result      = readBytes(_addr, reg, data, 2);
    if (result) {
        uint16_t value = data[0] | (data[1] << 8);
        *freq          = value;
    }
    return result;
}

bool M5UNIT_NCIR2::setAlarmBuzzInter(uint8_t index, uint16_t inter) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_ALARM_BUZZ_REG + index * 5 + 2;
    data[0]     = inter & 0xff;
    data[1]     = (inter >> 8) & 0xff;
    return writeBytes(_addr, reg, data, 2);
}

bool M5UNIT_NCIR2::getAlarmBuzzInter(uint8_t index, uint16_t *inter) {
    if (index > 1) return 0;

    uint8_t data[4];
    bool result;

    uint8_t reg = M5UNIT_NCIR2_ALARM_BUZZ_REG + index * 5 + 2;
    result      = readBytes(_addr, reg, data, 2);
    if (result) {
        uint16_t value = data[0] | (data[1] << 8);
        *inter         = value;
    }
    return result;
}

bool M5UNIT_NCIR2::setAlarmBuzzDuty(uint8_t index, uint8_t duty) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_ALARM_BUZZ_REG + index * 5 + 4;
    data[0]     = duty;
    return writeBytes(_addr, reg, data, 1);
}

bool M5UNIT_NCIR2::getAlarmBuzzDuty(uint8_t index, uint8_t *duty) {
    if (index > 1) return 0;

    uint8_t data[4];
    bool result;

    uint8_t reg = M5UNIT_NCIR2_ALARM_BUZZ_REG + index * 5 + 4;
    result      = readBytes(_addr, reg, data, 1);
    if (result) {
        uint8_t value = data[0];
        *duty         = value;
    }
    return result;
}

bool M5UNIT_NCIR2::setBuzzer(uint16_t freq, uint8_t duty) {
    uint8_t data[4];
    data[0] = freq & 0xff;
    data[1] = (freq >> 8) & 0xff;
    data[2] = duty;
    return writeBytes(_addr, M5UNIT_NCIR2_BUZZ_REG, data, 3);
}

bool M5UNIT_NCIR2::enable_buzz(void) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_BUZZ_REG + 3;

    data[0] = 1;

    return writeBytes(_addr, reg, data, 1);
}

bool M5UNIT_NCIR2::disable_buzz(void) {
    uint8_t data[4];
    uint8_t reg = M5UNIT_NCIR2_BUZZ_REG + 3;

    data[0] = 0;

    return writeBytes(_addr, reg, data, 1);
}

void M5UNIT_NCIR2::getBuzzer(uint16_t *freq, uint8_t *duty, uint8_t *enable) {
    uint8_t data[4];
    readBytes(_addr, M5UNIT_NCIR2_BUZZ_REG, data, 2);
    *freq = data[0] | (data[1] << 8);
    readBytes(_addr, M5UNIT_NCIR2_BUZZ_REG + 2, data, 1);
    *duty = data[0];
    readBytes(_addr, M5UNIT_NCIR2_BUZZ_REG + 3, data, 1);
    *enable = data[0];
}

void M5UNIT_NCIR2::setLEDColor(uint32_t color) {
    uint8_t data[4];
    data[2] = color & 0xff;
    data[1] = (color >> 8) & 0xff;
    data[0] = (color >> 16) & 0xff;
    writeBytes(_addr, M5UNIT_NCIR2_RGB_LED_REG, data, 3);
}

void M5UNIT_NCIR2::getLEDColor(uint32_t *color) {
    uint8_t data[4];
    bool result;

    result = readBytes(_addr, M5UNIT_NCIR2_RGB_LED_REG, data, 3);
    if (result) {
        uint32_t value = (data[0] << 16) | (data[1] << 8) | data[2];
        *color         = value;
    }
}

bool M5UNIT_NCIR2::getButtonStatus(void) {
    uint8_t data;
    readBytes(_addr, M5UNIT_NCIR2_BUTTON_REG, &data, 1);
    return data;
}

void M5UNIT_NCIR2::setConfig(void) {
    uint8_t data[4];
    data[0] = 1;
    writeBytes(_addr, M5UNIT_NCIR2_CONFIG_REG, data, 1);
    delay(1000);
}

uint8_t M5UNIT_NCIR2::getVersion(void) {
    uint8_t data = 0;
    readBytes(_addr, M5UNIT_NCIR2_FW_VERSION_REG, &data, 1);
    return data;
}
