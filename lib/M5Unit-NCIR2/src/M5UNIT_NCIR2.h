#ifndef _M5UNIT_NCIR2_H_
#define _M5UNIT_NCIR2_H_

#include <Arduino.h>
#include <Wire.h>

#define M5_NCIR_ALARM_LOW  0
#define M5_NCIR_ALARM_HIGH 1

#define M5UNIT_NCIR2_DEFAULT_ADDR   0x5A
#define M5UNIT_NCIR2_TEMP_REG       0x00

#define MLX90614_REGISTER_TA        0x06
#define MLX90614_REGISTER_TOBJ1	    0x07
#define MLX90614_REGISTER_TOBJ2	    0x08

#define M5UNIT_NCIR2_EMISSIVITY_REG 0x10
#define M5UNIT_NCIR2_ALARM_TEMP_REG 0x20
#define M5UNIT_NCIR2_ALARM_LED_REG  0x30
#define M5UNIT_NCIR2_ALARM_BUZZ_REG 0x40
#define M5UNIT_NCIR2_BUZZ_REG       0x50
#define M5UNIT_NCIR2_RGB_LED_REG    0x60
#define M5UNIT_NCIR2_BUTTON_REG     0x70
#define M5UNIT_NCIR2_CONFIG_REG     0x80
#define M5UNIT_NCIR2_SOC_TEMP_REG   0x90

#define M5UNIT_NCIR2_FW_VERSION_REG 0xFE

class M5UNIT_NCIR2 {
   private:
    uint8_t _addr;
    TwoWire *_wire;
    uint8_t _sda;
    uint8_t _scl;
    bool writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);
    bool readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);

   public:
    bool begin(TwoWire *wire = &Wire, uint8_t sda = 21, uint8_t scl = 22,
               uint8_t addr = M5UNIT_NCIR2_DEFAULT_ADDR);

    int16_t getTempValue(void);
    int16_t getSocTempValue(void);
    bool setEmissivity(float emissivity);
    float getEmissivity(void);
    bool setAlarmTemp(uint8_t index, int16_t temp);
    bool getAlarmTemp(uint8_t index, int16_t *temp);
    bool setAlarmLED(uint8_t index, uint32_t color);
    bool getAlarmLED(uint8_t index, uint32_t *color);
    bool setAlarmBuzzFreq(uint8_t index, uint16_t freq);
    bool getAlarmBuzzFreq(uint8_t index, uint16_t *freq);
    bool setAlarmBuzzInter(uint8_t index, uint16_t inter);
    bool getAlarmBuzzInter(uint8_t index, uint16_t *inter);
    bool setAlarmBuzzDuty(uint8_t index, uint8_t duty);
    bool getAlarmBuzzDuty(uint8_t index, uint8_t *duty);
    bool setBuzzer(uint16_t freq, uint8_t duty);
    bool enable_buzz(void);
    bool disable_buzz(void);
    void getBuzzer(uint16_t *freq, uint8_t *duty, uint8_t *enable);
    void setLEDColor(uint32_t color);
    void getLEDColor(uint32_t *color);
    bool getButtonStatus(void);
    void setConfig(void);
    uint8_t getVersion(void);
		int16_t readAmbient(void);
};

#endif
