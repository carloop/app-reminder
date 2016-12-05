#pragma once

#include "stdint.h"
#include "interval.h"

class Storage
{
public:
  bool verify();
  void load();

  static const char eepromMarker[];

  /* This struct must not be re-ordered since it is the EEPROM layout.
   * Elements must not be deleted.
   * To remove an element, replace the name by _unused1/2/3.
   * Elements must only be added at the end.
   */
  struct Data {
    char marker[4];
    uint8_t version;
  };

  static const Data DEFAULT_DATA;
};
