/* Run these tests on a laptop
 */
#include "catch.hpp"
#include "storage.h"
#include "application.h"

TEST_CASE("Storage::verify") {
  Storage storage;
  WHEN("EEPROM is cleared") {
    EEPROM.clear();
    THEN("storage gets initialized") {
      REQUIRE(storage.verify() == false);
    }
  }

  WHEN("Storage is already verified") {
    Storage dummy;
    dummy.verify();

    THEN("storage doesn't get reinitialized") {
      REQUIRE(storage.verify() == true);
    }
  }
}
