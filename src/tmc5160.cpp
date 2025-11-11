#include "tmc5160.hpp"
#include "spi_bus.hpp"

extern "C" {
#define TMC_NO_UART
#include "tmc/helpers/Constants.h"
#include "tmc/ic/TMC5160/TMC5160.h"
#include "tmc/ic/TMC5160/TMC5160_HW_Abstraction.h"
}

#include <iostream>

TMC5160::TMC5160(unsigned int cs_gpio) : cs_pin_(cs_gpio) {}

extern "C" void tmc5160_readWriteSPI(uint16_t, uint8_t* data, size_t length)
{
    // Este callback lo usará siempre el objeto activo; el CS se pasa vía TMC5160::cs_pin_
    extern unsigned int active_cs_pin;
    SPIBus::transfer(data, length, active_cs_pin);
}

extern "C" TMC5160BusType tmc5160_getBusType(uint16_t)
{
    return TMC5160BusType::IC_BUS_SPI;
}

unsigned int active_cs_pin = 0;

bool TMC5160::init()
{
    active_cs_pin = cs_pin_;
    icID_ = 0;
    std::cout << "TMC5160 inicializado en GPIO " << cs_pin_ << "\n";
    return true;
}

bool TMC5160::setSpeed(int motor_id, float rpm)
{
    (void)motor_id;
    active_cs_pin = cs_pin_;
    int32_t vmax = static_cast<int32_t>(rpm * 100);
    tmc5160_writeRegister(icID_, TMC5160_VMAX, vmax);
    return true;
}

float TMC5160::readPosition(int motor_id)
{
    (void)motor_id;
    active_cs_pin = cs_pin_;
    int32_t pos = 0;
    pos = tmc5160_readRegister(icID_, TMC5160_XACTUAL);
    return static_cast<float>(pos);
}


// Dummy UART, no implementado
extern "C" bool tmc5160_readWriteUART(uint16_t icID, uint8_t *data, size_t writeLength, size_t readLength) {return false;}
extern "C" uint8_t tmc5160_getNodeAddress(uint16_t icID) { return 0; }