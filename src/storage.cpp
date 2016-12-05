#include "storage.h"
#include "application.h"
#include "string.h"
#include "helpers.h"

const Storage::Data Storage::DEFAULT_DATA = {
  /* marker */ { 'C', 'L', 'R', 'M' },
  /* version */ 0,
};

bool Storage::verify()
{
  Data actual;
  EEPROM.get(0, actual);

  if (memcmp(actual.marker, DEFAULT_DATA.marker, member_sizeof(Data, marker)) != 0)
  {
    EEPROM.put(0, DEFAULT_DATA);
    return false;
  }
  else
  {
    return true;
  }
}

void Storage::load()
{
  //for (Interval &interval : intervals)
  //{

  //}

}

