cmake_minimum_required(VERSION 3.10)
project(tmc_driver LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED CATKIN_INSTALL_INTO_PREFIX_ROOT)
  message(STATUS "CATKIN_INSTALL_INTO_PREFIX_ROOT=${CATKIN_INSTALL_INTO_PREFIX_ROOT} (ignored; package uses ament)")
endif()

find_package(ament_cmake REQUIRED)

set(TMC_DRIVER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/spi_bus.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tmc5160.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tmc_driver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tmc_spi_registry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/TMC-API/tmc/ic/TMC5160/TMC5160.c
)

add_executable(test_drivers ${TMC_DRIVER_SOURCES})
target_compile_definitions(test_drivers PRIVATE TMC5160_IC_CACHE_COUNT=4)
target_include_directories(test_drivers
  PRIVATE
    include
    third_party/TMC-API
    third_party/TMC-API/tmc
    third_party/TMC-API/tmc/helpers
    third_party/TMC-API/tmc/ic
    third_party/TMC-API/tmc/ic/TMC5160
)

find_library(GPIOD_LIB gpiod)
if(NOT GPIOD_LIB)
  message(FATAL_ERROR "libgpiod not found, install with: sudo apt install libgpiod-dev")
endif()
target_link_libraries(test_drivers PRIVATE ${GPIOD_LIB})

install(TARGETS test_drivers
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_package()
