# Firmware - MainBoard

## Technology used

Card used : Zest_Core_STM32L4A6RG created by [6tron](https://6tron.io/)

Platform used :
- [PlatformIO](https://platformio.org/)
- [MBED](https://os.mbed.com/)

## Documents

You can find datasheet in `docs` folder.

## Cheatseet

```bash
pio run -t upload -e master
pio run -t upload -e robot
pio device monitor --eol LF
```