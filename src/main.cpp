#include "tmc_driver.hpp"
#include "tmc5160.hpp"
#include "spi_bus.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <utility>



int main()
{
    if (!SPIBus::init("/dev/spidev0.0", 1000000)) {
        std::perror("SPIBus::init");
        return 1;
    }

    // Crear instancias de los 4 drivers con sus pines reales
    TMC5160 drv1(5, 26);   // CS=GPIO5,  EN=GPIO26
    TMC5160 drv2(22, 19);  // CS=GPIO22, EN=GPIO19
    TMC5160 drv3(27, 24);  // CS=GPIO27, EN=GPIO24
    TMC5160 drv4(17, 23);  // CS=GPIO17, EN=GPIO23

    std::array<std::pair<const char*, TMCDriver*>, 4> drivers = {{
        {"DRV1", &drv1},
        {"DRV2", &drv2},
        {"DRV3", &drv3},
        {"DRV4", &drv4},
    }};

    for (auto& [label, drv] : drivers) {
        if (!drv->init()) {
            std::printf("[%s] Error al inicializar\n", label);
            continue;
        }
        const bool ok = drv->checkComms(label);
        std::printf("[%s] Resultado comunicación: %s\n", label, ok ? "OK" : "FALLO");
    }
    int32_t pos;

    for (auto& [label, drv] : drivers) {
        pos = drv->readPosition(0);
        std::printf("[%s] Posición leída: %d\n", label, pos);
    }

    // DRV2 speed to 10 rpm 
    drv2.setSpeed(0, 10.0f);
    // DRV3 speed to -15 rpm
    drv3.setSpeed(0, -15.0f);
    // DRV4 speed to 20 rpm
    drv4.setSpeed(0, 20.0f);


    while (true) {
        for (auto& [label, drv] : drivers) {
            pos = drv->readPosition(0);
            std::printf("[%s] Posición leída: %d\n", label, pos);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    SPIBus::close();
    return 0;
}
