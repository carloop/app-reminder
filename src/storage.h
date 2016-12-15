#pragma once

#include "stdint.h"

/* This struct must not be re-ordered since it is the EEPROM layout.
 * Elements must not be deleted.
 * To remove an element, replace the name by _unused1/2/3.
 * Elements must only be added at the end.
 */
struct Data {
  uint16_t marker;
  uint16_t version;

  double intervalCounter;
  double intervalLimit;
  uint8_t intervalReached;
};

class Storage
{
public:
  void load(Data &data);
  void store(Data &data);

private:
  static const Data DEFAULT_DATA;
};
