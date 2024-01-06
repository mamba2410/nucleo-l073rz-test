# NUCLEO-L073RZ Test

Testing out the dwo DO0180PFST05 screen with a NUCLEO-L073RZ board.

Using the STM32CubeL0 HAL and a cmake build chain so you don't need the Cube IDE.

All of this is on linux.

## Pinouts

### Screen

```
VBAT -- 3.3V (supply)
VCC -- 3.3V (supply)
IOVCC -- 3.3V (supply)
PWR_EN -- 3.3V
SPI_CS -- PB1
SPI_CLK -- PB13
SIO0 -- PB14 (MISO)
Q-SI1 -- PB15 (MOSI)
Q_SI2 -- x
Q_SI3 -- x
RESET -- PB2
TP_RESET -- 3.3V
TP_SCL -- x
TP_SDA -- x
TP_INT -- x
IM1 -- GND (SPI 3-wire)
TE --x
GND -- GND
```

## Flashing and Debugging with st-link

Holy moly this was annoying to figure out.


### Build `stutil`

Get the `stutil` head from [GitHub](https://github.com/stlink-org/stlink).

Build with

```
make release
```

Confirmed working with version `v1.7.0-314-g40fbdb4`

I can't get `make install` to work properly :/

### Connections

- Make sure `CN2` st-link debug headers are bridged. Top two and bottom two.
- Connect via USB


### Debugging

From `stutil` root

```
./build/Release/bin/st-util
```

This opens a GDB server on port :4242


OR with `openocd`

```
openocd -f board/st_nucleo_l073rz.cfg
```

From this project root, run

```
arm-none-eabi-gdb ./build/stm32-blinky-l0.elf
```

Now in GDB

```
(gdb) target remote :4242
(gdb) load
```

It *should* load the elf file.
Can then run normal gdb commands from there.

### Notes on flashing/debugging

openocd doesn't seem to work well as a gdb server.
I get multiple different errors trying to flash the program.

Tried connecting with USB and running

```
openocd -f interface/stlink.cfg -f board/st_nucleo_l073rz.cfg
```

Or connecting via swd (remove jumpers on `CN2`, SWDIO on pin 4 (from the top) and SWCLK on pin 2)

```
openocd -f ./ft2232h.cfg -f target/stm32l0.cfg
```

Neither work, i get access denied and stuff like that.
Suuuuper fun, and googling it seems to be inconclusive, just people saying the bug has now been fixed,
but clearly it hasn't.
Or it's a skill issue on my end.

