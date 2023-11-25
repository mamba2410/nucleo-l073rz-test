#include <stdint.h>
#include <stddef.h>

/*
 * DWO DO0180PFST05 AMOLED screen via STM32L073RZ
 *
 * Run screen in SPI 3-wire mode (SCK, MOSI, MISO) as opposed to 4-wire (SCK, MOSI, MISO, D/C)
 *
 *
 */

#include "stm32l0xx_hal.h"
#include <stm32l0xx_ll_spi.h>   // for spi helpers
#include <stm32l0xx_ll_gpio.h>  // for gpio defs
#include <stm32l0xx_ll_bus.h>   // For clock enables

#define SCREEN_CSB_PORT GPIOB
#define SCREEN_CSB_PIN  LL_GPIO_PIN_1

#define SPI_SCK_PIN LL_GPIO_PIN_13
#define SPI_SCK_PORT GPIOB
#define SPI_MOSI_PIN LL_GPIO_PIN_15
#define SPI_MOSI_PORT GPIOB
#define SPI_MISO_PIN LL_GPIO_PIN_14
#define SPI_MISO_PORT GPIOB

#define SCREEN_WIDTH    96
#define SCREEN_HEIGHT   64
#define SCREEN_SCALE    4
#define BYTES_PER_PIXEL 3

uint8_t screen_line_buffer[SCREEN_WIDTH*SCREEN_SCALE*BYTES_PER_PIXEL];

static SPI_TypeDef *screen_spi = 0;

void screen_csb_enable() {
    LL_GPIO_ResetOutputPin(SCREEN_CSB_PORT, SCREEN_CSB_PIN);
}

void screen_csb_disable() {
    LL_GPIO_SetOutputPin(SCREEN_CSB_PORT, SCREEN_CSB_PIN);
}

void screen_write_pixels_1wire(SPI_TypeDef *hspi, size_t len, uint8_t data[len]) {

    screen_csb_enable();
    LL_SPI_TransmitData8(hspi, 0x02);   // Command write, 1-wire
    LL_SPI_TransmitData8(hspi, 0x00);
    LL_SPI_TransmitData8(hspi, 0x2c);   // either 0x2c or 0x3c, idk the difference
    LL_SPI_TransmitData8(hspi, 0x00);

    // write pixel data
    // interlaced R8 G8 B8 R8...
    for(size_t i = 0; i < len; i++) {
        LL_SPI_TransmitData8(hspi, data[i]);
    }

    screen_csb_disable();
}

/*
 * Write command to the screen. Format is a little weird, but load `data` with all the args
 */
void screen_cmd_write(uint8_t cmd, size_t len, uint8_t data[len]) {
    screen_csb_enable();

    LL_SPI_TransmitData8(screen_spi, 0x02);
    LL_SPI_TransmitData8(screen_spi, 0x00);
    LL_SPI_TransmitData8(screen_spi, cmd);
    LL_SPI_TransmitData8(screen_spi, 0x00);

    for(size_t i = 0; i < len; i++) {
        LL_SPI_TransmitData8(screen_spi, data[i]);
    }

    screen_csb_disable();
}

void screen_reg_write(SPI_TypeDef *hspi, uint32_t addr, size_t len, uint8_t data[len]) {
    screen_csb_enable();
    LL_SPI_TransmitData8(hspi, 0x02);   // command reg write
    LL_SPI_TransmitData8(hspi, (addr>>16)&0xff);
    LL_SPI_TransmitData8(hspi, (addr>>8)&0xff);
    LL_SPI_TransmitData8(hspi, addr&0xff);

    for(size_t i = 0; i < len; i++) {
        LL_SPI_TransmitData8(hspi, data[i]);
    }
    screen_csb_disable();
}

void screen_reg_read(SPI_TypeDef *hspi, uint32_t addr, size_t len, uint8_t data[len]) {
    screen_csb_enable();
    LL_SPI_TransmitData8(hspi, 0x02);   // command reg write
    LL_SPI_TransmitData8(hspi, (addr>>16)&0xff);
    LL_SPI_TransmitData8(hspi, (addr>>8)&0xff);
    LL_SPI_TransmitData8(hspi, addr&0xff);

    for(size_t i = 0; i < len; i++) {
        data[i] = LL_SPI_ReceiveData8(hspi);
    }
    screen_csb_disable();
}

/*
 * Configure SPI1 as `screen_spi`
 * Run screen reset sequence
 */
void spi_init() {
    // enable peripheral clock for port B
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    // SCK, pins and functions need changing, but this is good for now
    LL_GPIO_SetPinMode(SPI_SCK_PORT, SPI_SCK_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(SPI_SCK_PORT, SPI_SCK_PIN, LL_GPIO_AF_0);
    LL_GPIO_SetPinSpeed(SPI_SCK_PORT, SPI_SCK_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(SPI_SCK_PORT, SPI_SCK_PIN, LL_GPIO_PULL_DOWN);

    // MOSI, pins and functions need changing, but this is good for now
    LL_GPIO_SetPinMode(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_AF_0);
    LL_GPIO_SetPinSpeed(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(SPI_MOSI_PORT, SPI_MOSI_PIN, LL_GPIO_PULL_DOWN);

    // MISO, pins and functions need changing, but this is good for now
    LL_GPIO_SetPinMode(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_AF_0);
    LL_GPIO_SetPinSpeed(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(SPI_MISO_PORT, SPI_MISO_PIN, LL_GPIO_PULL_DOWN);

    // CSB, pins and functions need changing, but this is good for now
    //LL_GPIO_SetPinMode(SCREEN_CSB_PORT, SCREEN_CSB_PIN, LL_GPIO_MODE_ALTERNATIVE);
    //LL_GPIO_SetAFPin_0_7(SCREEN_CSB_PORT, SCREEN_CSB_PIN, LL_GPIO_AF_0);
    LL_GPIO_SetPinSpeed(SCREEN_CSB_PORT, SCREEN_CSB_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(SCREEN_CSB_PORT, SCREEN_CSB_PIN, LL_GPIO_PULL_DOWN);

    // configure SPI1

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV32);
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
    LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
    LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
    LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT); // software CSB
    LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

    screen_spi = SPI1;

    /*
     * Initialise screen
     * Adapted from `AMOLED_Init` function in `Display_Demo_Code` file
     * For detailed command descriptions, see SH8601Z0 datasheet section 5
     */
    uint8_t buf[4];
    screen_cmd_write(0x11, 0, buf); // sleep out, no args
    screen_cmd_write(0x34, 1, buf); // TE off
    buf[0] = 0x77;
    screen_cmd_write(0x3a, 1, buf); // pixel format 24bpp (RGB888)
    buf[0] = 0x20;
    screen_cmd_write(0x53, 0, buf); // "Write CTRL DIsplay1", brightness control on, display dimming off

    HAL_Delay(10);

    buf[0] = 0x00;
    screen_cmd_write(0x51, 1, buf); // screen brightness

    HAL_Delay(10);

    screen_cmd_write(0x29, 0, buf); // screen on
    buf[0] = 0xff;
    screen_cmd_write(0x51, 1, buf); // screen brightness

}

void screen_set_draw_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint8_t buf[4];

    uint16_t xend = x+w-1;
    uint16_t yend = y+w-1;

    buf[0] = x>>8;
    buf[1] = x&0xff;
    buf[2] = xend>>8;
    buf[3] = xend&0xff;
    screen_cmd_write(0x2a, 4, buf); // set column start

    buf[0] = y>>8;
    buf[1] = y&0xff;
    buf[2] = yend>>8;
    buf[3] = yend&0xff;
    screen_cmd_write(0x2b, 4, buf); // set row start

    // now call `screen_write_pixels`
}

int main() {

    //SystemClock_Config();
    HAL_Init();

    spi_init();

    screen_set_draw_area(0, 0, 8, 8);
    for(size_t i = 0; i < 64; i++) {
        screen_line_buffer[3*i+0] = 0xff;
        screen_line_buffer[3*i+1] = 0xff;
        screen_line_buffer[3*i+2] = 0xff;
    }
    screen_write_pixels_1wire(screen_spi, 64, screen_line_buffer);

    while(1) {

    }
}
