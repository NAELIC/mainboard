#include "common.h"

SHELL_COMMAND(mode, "CHANGE MODE")
{
    if (argc == 1)
    {
        shell_println(argv[0]);
        common::mode = atoi(argv[0]) == 0 ? Mode::NORMAL : Mode::DEBUG;
    }
}