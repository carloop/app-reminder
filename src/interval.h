#include "stdint.h"

class Interval
{
  public:
    Interval(uint8_t index);

    uint8_t getIndex();
    uint32_t getValue();
    uint32_t getLimit();

    void setValue(uint32_t newValue);
    void addValue(uint32_t increment);
    void setLimit(uint32_t newLimit);

    bool limitReached();

  private:
    uint8_t index;
    uint32_t value;
    uint32_t limit;
};
