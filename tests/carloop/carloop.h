// Carloop stubs for tests
#pragma once
#include "../application.h"

struct CarloopRevision2
{
};

template <typename Config>
class Carloop
{
public:
    Carloop();

    void setCANSpeed(uint32_t canSpeed);
    void begin();
    CANChannel &can();
};

