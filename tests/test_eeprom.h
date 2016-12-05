// Particle EEPROM implementation for the tests
#include "stdint.h"

#define EEPROM_SIZE 4096

class EEPROMClass
{
public:
  void clear();
  uint8_t read(uint16_t idx);
  void write(uint16_t idx, uint8_t value);

  void get(uint16_t idx, void *value, uint16_t size);
  void put(uint16_t idx, void *value, uint16_t size);

  template <typename T> T &get( int idx, T &t )
  {
    get(idx, (void *)&t, sizeof(T));
    return t;
  }

  template <typename T> const T &put( int idx, const T &t )
  {
    put(idx, (void *)&t, sizeof(T));
    return t;
  }

private:
  uint8_t storage[EEPROM_SIZE];
};

// Global instance
extern EEPROMClass EEPROM;
