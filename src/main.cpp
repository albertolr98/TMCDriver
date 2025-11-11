#include "tmc5160.hpp"
#include "spi_bus.hpp"
#include <cstdio>

int main()
{
    if (!SPIBus::init("/dev/spidev0.0", 1000000)) return 1;

    TMC5160 driverA(17); // GPIO17
    TMC5160 driverB(27); // GPIO27
    TMC5160 driverC(22); // GPIO22

    driverA.init();
    driverB.init();
    driverC.init();

    driverA.setSpeed(0, 100.0);
    driverB.setSpeed(0, 200.0);
    driverC.setSpeed(0, 300.0);

    std::printf("Pos A: %.2f\n", driverA.readPosition(0));
    std::printf("Pos B: %.2f\n", driverB.readPosition(0));
    std::printf("Pos C: %.2f\n", driverC.readPosition(0));

    SPIBus::close();
    return 0;
}
