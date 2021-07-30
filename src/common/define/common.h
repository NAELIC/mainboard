#pragma once

#include <swo.h>
#include <shell.h>

enum Mode
{
    NORMAL,
    DEBUG
};

namespace common
{

    static naelic::SWO swo;
    extern Mode mode;
}