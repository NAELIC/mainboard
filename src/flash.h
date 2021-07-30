#pragma once

#include <stddef.h>
#include <stdint.h>

void flash_read(uint8_t *target, uint32_t source, size_t size);
void flash_write(uint32_t target, uint64_t *source, size_t size);