#include "interval.h"

Interval::Interval(uint8_t index)
  : index(index), value(0), limit(0)
{

}

uint8_t Interval::getIndex()
{
  return index;
}

uint32_t Interval::getValue()
{
  return value;
}

uint32_t Interval::getLimit()
{
  return limit;
}

void Interval::setValue(uint32_t newValue)
{
  value = newValue;
}

void Interval::addValue(uint32_t increment)
{
  value += increment;
}

void Interval::setLimit(uint32_t newLimit)
{
  limit = newLimit;
}

bool Interval::limitReached()
{
  return limit != 0 && value >= limit;
}
