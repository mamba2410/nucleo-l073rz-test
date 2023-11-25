cmake_minimum_required(VERSION 3.16)

set(STM32_CUBE_L0_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules/STM32CubeL0/)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/modules/stm32-cmake/cmake/stm32_gcc.cmake)


project(stm32-blinky C ASM)
set(MAIN_SOURCE_FILE src/screen_ll.c)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)


set(HAL_COMP_LIST RCC GPIO CORTEX STM32L0)
set(CMSIS_COMP_LIST STM32L0)

find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)


add_executable(stm32-blinky-l0 ${MAIN_SOURCE_FILE} stm32l0xx_hal_conf.h)
target_link_libraries(stm32-blinky-l0
    HAL::STM32::L0::RCC
    HAL::STM32::L0::GPIO
    HAL::STM32::L0::CORTEX
    CMSIS::STM32::L073RZ
    STM32::NoSys
)
stm32_print_size_of_target(stm32-blinky-l0)

