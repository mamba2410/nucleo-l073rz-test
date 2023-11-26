#ifndef SCREEN_H
#define SCREEN_H

#include "stm32l0xx_hal.h"

#define SCREEN_SPI                          SPI2

#define SCREEN_SCK_PIN                      GPIO_PIN_13
#define SCREEN_SCK_PORT                     GPIOB
#define SCREEN_SCK_AF                       GPIO_AF0_SPI2

#define SCREEN_MISO_PIN                     GPIO_PIN_14
#define SCREEN_MISO_PORT                    GPIOB
#define SCREEN_MISO_AF                      GPIO_AF0_SPI2

#define SCREEN_MOSI_PIN                     GPIO_PIN_15
#define SCREEN_MOSI_PORT                    GPIOB
#define SCREEN_MOSI_AF                      GPIO_AF0_SPI2

#define SCREEN_CSB_PORT                     GPIOB
#define SCREEN_CSB_PIN                      GPIO_PIN_1

#define SCREEN_SPI                          SPI2
#define SCREEN_SPI_CLK_ENABLE()             __HAL_RCC_SPI2_CLK_ENABLE()
#define SCREEN_SPI_SCK_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define SCREEN_SPI_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define SCREEN_SPI_MISO_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define SCREEN_SPI_CSB_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()


#define SCREEN_SPI_FORCE_RESET()            __HAL_RCC_SPI2_FORCE_RESET()
#define SCREEN_SPI_RELEASE_RESET()          __HAL_RCC_SPI2_RELEASE_RESET()


#endif /* SCREEN_H */
