#include "storage.h"
#include "application.h"
#include "string.h"
#include "helpers.h"

const Data Storage::DEFAULT_DATA = {
  /* marker */ 0x4352, // Letters CR = Carloop Reminder
  /* version */ 0,
  /* intervalCounter */ 0.0,
  /* intervalLimit */ 5000.0,
  /* intervalReached */ 0,
};

void Storage::load(Data &data)
{
  EEPROM.get(0, data);

  // On first load, set the EEPROM to default values
  if (data.marker != DEFAULT_DATA.marker)
  {
    data = DEFAULT_DATA;
    store(data);
  }
}

void Storage::store(Data &data)
{
    EEPROM.put(0, data);
}

