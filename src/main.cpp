#include "tmc_driver.hpp"
#include "tmc5160.hpp"
#include "spi_bus.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <utility>

namespace {
std::atomic<bool> g_shouldStop{false};

void handleSigint(int)
{
    g_shouldStop.store(true);
}

void runHardcodedSquare(const std::array<std::pair<const char*, TMCDriver*>, 4>& drivers)
{
    struct Segment {
        float drv2_rpm;
        float drv3_rpm;
        float drv4_rpm;
        std::chrono::milliseconds duration;
        const char* description;
    };

    const std::array<Segment, 4> pattern = {{
        { 10.0f,  10.0f,   0.0f, std::chrono::milliseconds(1500), "Segmento 1: adelante"},
        {  5.0f, -10.0f,  10.0f, std::chrono::milliseconds(1500), "Segmento 2: derecha"},
        {-10.0f, -10.0f,   0.0f, std::chrono::milliseconds(1500), "Segmento 3: atrás"},
        { -5.0f,  10.0f, -10.0f, std::chrono::milliseconds(1500), "Segmento 4: izquierda"},
    }};

    while (!g_shouldStop.load()) {
        for (const auto& segment : pattern) {
            if (g_shouldStop.load()) break;

            drivers[1].second->setSpeed(0, segment.drv2_rpm);
            drivers[2].second->setSpeed(0, segment.drv3_rpm);
            drivers[3].second->setSpeed(0, segment.drv4_rpm);
            std::printf("%s | v2=%.1f rpm, v3=%.1f rpm, v4=%.1f rpm\n",
                        segment.description, segment.drv2_rpm, segment.drv3_rpm, segment.drv4_rpm);

            const auto start = std::chrono::steady_clock::now();
            while (!g_shouldStop.load()) {
                const auto elapsed = std::chrono::steady_clock::now() - start;
                if (elapsed >= segment.duration) break;

                for (auto& [label, drv] : drivers) {
                    const auto pos = drv->readPosition(0);
                    std::printf("[%s] Posición leída: %.2f\n", label, pos);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        }
    }
}
}  // namespace


int main()
{
    std::signal(SIGINT, handleSigint);

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
    for (auto& [label, drv] : drivers) {
        const auto pos = drv->readPosition(0);
        std::printf("[%s] Posición leída: %.2f\n", label, pos);
    }

    runHardcodedSquare(drivers);

    for (auto& [label, drv] : drivers) {
        drv->setSpeed(0, 0.0f);
        drv->shutdown();
        std::printf("[%s] Driver deshabilitado\n", label);
    }

    SPIBus::close();
    return 0;
}
