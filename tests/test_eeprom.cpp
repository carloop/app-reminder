#include "test_eeprom.h"

void EEPROMClass::clear()
{
  for (uint16_t i = 0; i < EEPROM_SIZE; i++)
  {
    storage[i] = 0xFF;
  }
}

uint8_t EEPROMClass::read(uint16_t idx)
{
  return storage[idx];
}

void EEPROMClass::write(uint16_t idx, uint8_t value)
{
  storage[idx] = value;
}

void EEPROMClass::get(uint16_t idx, void *value, uint16_t size)
{
  for (uint16_t i = 0; i < size; i++)
  {
    ((uint8_t *)value)[i] = read(idx + i);
  }
}

void EEPROMClass::put(uint16_t idx, void *value, uint16_t size)
{
  for (uint16_t i = 0; i < size; i++)
  {
    write(idx + i, ((uint8_t *)value)[i]);
  }
}

// Global instance
EEPROMClass EEPROM;
