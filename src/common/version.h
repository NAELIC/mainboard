#pragma once

#include <shell.h>

#ifndef COMMIT
#define COMMIT NONE
#endif

#ifndef CARD
#define CARD NONE
#endif

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#define FIRMWARE_VERSION() VALUE(COMMIT)
#define CARD_VERSION() VALUE(CARD)

SHELL_COMMAND(version, "version")
{
  shell_print("Card : ");
  shell_println(CARD_VERSION());
  shell_print("Firmware : ");
  shell_println(FIRMWARE_VERSION());
}