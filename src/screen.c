#include <stdint.h>
#include <stddef.h>

#include "screen.h"

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_spi.h"
//#include "stm32l0xx_nucleo.h"

#define SCREEN_WIDTH    96
#define SCREEN_HEIGHT   64
#define SCREEN_SCALE    4
#define BYTES_PER_PIXEL 3

void SystemClock_Config(void);

SPI_HandleTypeDef SCREEN_SPI_HANDLE = {0};

uint8_t screen_line_buffer[SCREEN_WIDTH*SCREEN_SCALE*BYTES_PER_PIXEL];

/**
 * Goals:
 *  - SPI 3-wire: full-duplex. mosi, miso, sck (no d/c)
 *
 * Code stolen from https://community.st.com/t5/stm32-mcus-products/spi-half-duplex-example/td-p/257333
 *
 */


HAL_StatusTypeDef screen_spi_init() {

    HAL_StatusTypeDef ret = HAL_OK;

    SCREEN_SPI_HANDLE.Instance = SCREEN_SPI;
    SCREEN_SPI_HANDLE.Init.Mode = SPI_MODE_MASTER;
    SCREEN_SPI_HANDLE.Init.Direction = SPI_DIRECTION_1LINE;
    SCREEN_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;
    SCREEN_SPI_HANDLE.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SCREEN_SPI_HANDLE.Init.CLKPhase = SPI_PHASE_2EDGE;
    SCREEN_SPI_HANDLE.Init.NSS = SPI_NSS_SOFT;
    SCREEN_SPI_HANDLE.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    SCREEN_SPI_HANDLE.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SCREEN_SPI_HANDLE.Init.TIMode = SPI_TIMODE_DISABLE;
    SCREEN_SPI_HANDLE.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SCREEN_SPI_HANDLE.Init.CRCPolynomial = 7;
    //hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLED;

    if( HAL_SPI_Init(&SCREEN_SPI_HANDLE) != HAL_OK ) {
        ret = HAL_ERROR;
    }

    SCREEN_SPI_CSB_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin       = SCREEN_CSB_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SCREEN_CSB_PORT, &GPIO_InitStruct);

    return ret;
}

void screen_write_cmd(uint8_t cmd, size_t len, uint8_t params[len]) {
    uint8_t buf[] = {0x02, 0x00, cmd, 0x00};

    HAL_GPIO_WritePin(SCREEN_CSB_PORT, SCREEN_CSB_PIN, GPIO_PIN_RESET );
    HAL_SPI_Transmit(&SCREEN_SPI_HANDLE, buf, 4, 15);
    if(len > 0)
        HAL_SPI_Transmit(&SCREEN_SPI_HANDLE, params, len, 15);
    HAL_GPIO_WritePin(SCREEN_CSB_PORT, SCREEN_CSB_PIN, GPIO_PIN_SET);
}

void screen_reset_sequence() {
    uint8_t params[4] = {0};

    /*
     * Initialise screen
     * Adapted from `AMOLED_Init` function in `Display_Demo_Code` file
     * For detailed command descriptions, see SH8601Z0 datasheet section 5
     */
    screen_write_cmd(0x11, 0, params); // sleep out, no args
    screen_write_cmd(0x34, 0, params); // TE off
    params[0] = 0x77;
    screen_write_cmd(0x3a, 1, params); // pixel format 24bpp (RGB888)
    params[0] = 0x20;
    screen_write_cmd(0x53, 0, params); // "Write CTRL DIsplay1", brightness control on, display dimming off

    HAL_Delay(10);

    params[0] = 0x00;
    screen_write_cmd(0x51, 1, params); // screen brightness

    HAL_Delay(10);

    screen_write_cmd(0x29, 0, params); // screen on
    params[0] = 0xff;
    screen_write_cmd(0x51, 1, params); // screen brightness

}

void screen_draw_1wire(size_t len, uint8_t data[len]) {

    screen_write_cmd(0x2c, len, data);
}

int main() {
    HAL_Init();
    SystemClock_Config();

    screen_spi_init();
    screen_reset_sequence();

    for(size_t i = 0; i < 64*3; i++) {
        screen_line_buffer[i] = 0xff;
    }

    screen_draw_1wire(64*3, screen_line_buffer);


    while(1);
    return 0;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = MSI
  *            SYSCLK(Hz)                     = 2000000
  *            HCLK(Hz)                       = 2000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            Flash Latency(WS)              = 0
  *            Main regulator output voltage  = Scale3 mode
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable MSI Oscillator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.MSICalibrationValue=0x00;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  //__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

}

