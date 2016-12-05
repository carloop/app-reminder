// Carloop stubs for tests
#include "carloop.h"

CANChannel theChannel;

template<typename Config>
Carloop<Config>::Carloop()
{
}

template <typename Config>
void Carloop<Config>::setCANSpeed(uint32_t canSpeed)
{
}

template <typename Config>
void Carloop<Config>::begin()
{
}

template <typename Config>
CANChannel &Carloop<Config>::can()
{
  return theChannel;
}

// Template instantiation
template class Carloop<CarloopRevision2>;
