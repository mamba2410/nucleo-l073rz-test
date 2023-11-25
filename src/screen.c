#include <stdint.h>
#include <stddef.h>

#include "stm32l0xx_hal.h"

#define SCREEN_CSB_PORT GPIOB       // TODO: change this
#define SCREEN_CSB_PIN  GPIO_PIN_5  // TODO: change this

#define SCREEN_WIDTH    96
#define SCREEN_HEIGHT   64
#define SCREEN_SCALE    4
#define BYTES_PER_PIXEL 3


uint8_t screen_line_buffer[SCREEN_WIDTH*SCREEN_SCALE*BYTES_PER_PIXEL];

/**
 * Goals:
 * - Write to screen over half-duplex SPI (spi 3-wire)
 *
 * Code stolen from https://community.st.com/t5/stm32-mcus-products/spi-half-duplex-example/td-p/257333
 *
 */


HAL_StatusTypeDef spi2_init(SPI_HandleTypeDef *hspi) {

    HAL_StatusTypeDef ret = HAL_OK;

    hspi->Instance = SPI2;
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_1LINE;
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    //hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLED;

    if( HAL_SPI_Init(hspi) != HAL_OK ) {
        ret = HAL_ERROR;
    }

    HAL_Delay(5);
    SPI_1LINE_TX(hspi);
    HAL_Delay(5);
    __HAL_SPI_ENABLE(hspi);

    return ret;
}

void screen_write_cmd(SPI_HandleTypeDef *hspi, uint8_t cmd) {
    HAL_GPIO_WritePin(SCREEN_CSB_PORT, SCREEN_CSB_PIN, GPIO_PIN_RESET );
    HAL_SPI_Transmit(hspi, &cmd, 1, 15);
    HAL_GPIO_WritePin(SCREEN_CSB_PORT, SCREEN_CSB_PIN, GPIO_PIN_SET);
}

void screen_read_reg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *setting) {
    uint8_t read_mask;
    uint8_t data;

    HAL_GPIO_WritePin(SCREEN_CSB_PORT, SCREEN_CSB_PIN, GPIO_PIN_RESET);
    read_mask = reg | 0x80;
    //                SPI,  data buffer, size, timeout
    HAL_SPI_Transmit(hspi,   &read_mask,     1,     15);
    __HAL_SPI_DISABLE(hspi);
    SPI_1LINE_RX(hspi);
    __HAL_SPI_ENABLE(hspi);
    HAL_SPI_Receive(hspi, &data, 1, 15);
    __DSB();
    __DSB();
    __HAL_SPI_DISABLE(hspi);
    HAL_GPIO_WritePin(SCREEN_CSB_PORT, SCREEN_CSB_PIN, GPIO_PIN_SET);
    SPI_1LINE_TX(hspi);
    __HAL_SPI_ENABLE(hspi);

}

void screen_init(SPI_HandleTypeDef *hspi) {
    // stolen from `DO0180PFST05_Display_Demo Code(QSPI).c`

    screen_write_cmd(hspi, 0x11); // sleep out
    HAL_Delay(120);



}

int main() {
    HAL_Init();



    while(1);

    return 0;

}


