#include "tmc5160.hpp"
#include "spi_bus.hpp"
#include <cstdio>

int main()
{
    // Inicializar bus SPI compartido
    SPIBus::init("/dev/spidev0.0", 1000000);

    // Crear instancias de los 4 drivers con sus pines reales
    TMC5160 drv1(5, 26);   // CS=GPIO5,  EN=GPIO26
    TMC5160 drv2(22, 19);  // CS=GPIO22, EN=GPIO19
    TMC5160 drv3(27, 24);  // CS=GPIO27, EN=GPIO24
    TMC5160 drv4(17, 23);  // CS=GPIO17, EN=GPIO23

    drv1.init();
    drv2.init();
    drv3.init();
    drv4.init();

    drv1.setSpeed(0, 120.0);
    drv2.setSpeed(0, 150.0);
    drv3.setSpeed(0, 180.0);
    drv4.setSpeed(0, 100.0);

    std::printf("Pos D1: %.2f\n", drv1.readPosition(0));
    std::printf("Pos D2: %.2f\n", drv2.readPosition(0));
    std::printf("Pos D3: %.2f\n", drv3.readPosition(0));
    std::printf("Pos D4: %.2f\n", drv4.readPosition(0));

    SPIBus::close();
    return 0;
}
