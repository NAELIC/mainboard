#include "infos.h"
#include "hardware.h"
#include "mux.h"
#include "shell.h"
#include "flash.h"
#include <mbed.h>
#include <stdlib.h>

// Flash address to write/read infos
#define INFOS_FLASH_PAGE 511
#define INFOS_FLASH_ADDR 0x080FF800
bool developer_mode = false;

int IDs[11] = {7, 0,  1, 8, 2, 4,
               9, 10, 3, 5, 6}; // The coding of IDs from hall values

int id_from_hall(bool hall1, bool hall2, bool hall3, bool hall4) {
  int index = (hall1) + (hall2 << 1) + (hall3 << 2) + (hall4 << 3);
  if (index < 11) {
    return IDs[index];
  } else {
    return -1;
  }
}

struct robot_infos {
  int id;
  bool kickerInverted;
};

static struct robot_infos infos;

void infos_init() {
  infos.id = 1;

  // uint8_t *a = (uint8_t*)&infos;
  // uint8_t *b = (uint8_t*)(INFOS_FLASH_ADDR);
  // for (int k=0; k<sizeof(struct robot_infos); k++) {
  //   a[k] = b[k];
  // }

  flash_read((uint8_t *)&infos, INFOS_FLASH_ADDR, sizeof(struct robot_infos));
}

int infos_get_id() { return infos.id; }

bool infos_kicker_inverted() { return infos.kickerInverted; }

SHELL_COMMAND(infos, "ID") {
  shell_println("ID:");
  shell_println(infos.id);

  shell_println("Kicker inverted:");
  shell_println((int)infos.kickerInverted);
}

void infos_set(int id, bool kickerInverted) {
  infos.id = id;
  infos.kickerInverted = kickerInverted;

  flash_write(INFOS_FLASH_ADDR, (uint64_t *)&infos, sizeof(struct robot_infos));
}
