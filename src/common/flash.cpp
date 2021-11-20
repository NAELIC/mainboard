#include "flash.h"
#include <mbed.h>

void flash_read(uint8_t *target, uint32_t source, size_t size) {
  memcpy((void *)target, (void *)source, size);
}

void flash_write(uint32_t target, uint64_t *source, size_t size) {
  ScopedRomWriteLock lock;
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef flashErase;
  flashErase.TypeErase = TYPEERASE_PAGEERASE;
  flashErase.Page = 511;
  flashErase.NbPages = 1;
  uint32_t error;

  if (HAL_FLASHEx_Erase(&flashErase, &error) == HAL_OK) {
    int doublewords = (size / 8) + 1;

    for (int k = 0; k < doublewords; k++) {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, target + 8 * k,
                        source[k]);
    }
  }

  HAL_FLASH_Lock();
}