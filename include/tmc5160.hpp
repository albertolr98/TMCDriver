#pragma once
#include "tmc_driver.hpp"
#include <cstdint>

class TMC5160 : public TMCDriver {
public:
    TMC5160(unsigned int cs_gpio);
    bool init() override;
    bool setSpeed(int motor_id, float rpm) override;
    float readPosition(int motor_id) override;

private:
    uint8_t icID_ = 0;
    unsigned int cs_pin_;
};
