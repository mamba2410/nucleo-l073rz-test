/**
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_ComPolling/Src/stm32l0xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include "stm32l0xx_hal.h"
#include "screen.h"

/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @defgroup SPI_FullDuplex_AdvComPolling
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
GPIO_InitTypeDef  GPIO_InitStruct;

  if(hspi->Instance == SCREEN_SPI)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    SCREEN_SPI_SCK_GPIO_CLK_ENABLE();
    SCREEN_SPI_MISO_GPIO_CLK_ENABLE();
    SCREEN_SPI_MOSI_GPIO_CLK_ENABLE();
    /* Enable SPI clock */
    SCREEN_SPI_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SCREEN_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = SCREEN_SCK_AF;
    HAL_GPIO_Init(SCREEN_SCK_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = SCREEN_MISO_PIN;
    GPIO_InitStruct.Alternate = SCREEN_MISO_AF;
    HAL_GPIO_Init(SCREEN_MISO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SCREEN_MOSI_PIN;
    GPIO_InitStruct.Alternate = SCREEN_MOSI_AF;
    HAL_GPIO_Init(SCREEN_MOSI_PORT, &GPIO_InitStruct);
  }
}

/**
  * @brief SPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to its default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if(hspi->Instance == SCREEN_SPI)
  {
    /*##-1- Reset peripherals ##################################################*/
    SCREEN_SPI_FORCE_RESET();
    SCREEN_SPI_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(SCREEN_SCK_PORT, SCREEN_SCK_PIN);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(SCREEN_MISO_PORT, SCREEN_MISO_PIN);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(SCREEN_MOSI_PORT, SCREEN_MOSI_PIN);
  }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
