/* Run these tests on a laptop
 */
#include "catch.hpp"

// On the Photon, double is actually the same length as a float
#define double float

// Include the actual code for the application in here!
#include "../src/app-reminder.cpp"

TEST_CASE("setup") {
  setup();

  // setup initializes storage
  REQUIRE(data.appId == DEFAULT_DATA.appId);
}

TEST_CASE("computeDeltaMileage") {
  const auto eps = 0.000001;
  setup();

  SECTION("100 ms at 74 km/h (~45 mph) is 0.001277 miles") {
    uint8_t speed = 74; // km/h
    timeTravel(0);
    double delta = computeDeltaMileage(speed);
    REQUIRE(delta == 0.0);
    timeTravel(100);
    delta = computeDeltaMileage(speed);
    REQUIRE(delta == Approx(0.001277).epsilon(eps));
  }

  SECTION("100 ms at 4 km/h (~2 mph) is 0.000069 miles") {
    uint8_t speed = 4; // km/h
    timeTravel(0);
    double delta = computeDeltaMileage(speed);
    REQUIRE(delta == 0.0);
    timeTravel(100);
    delta = computeDeltaMileage(speed);
    REQUIRE(delta == Approx(0.000069).epsilon(eps));
  }
}

double distanceFromSpeed(uint8_t speed, uint16_t seconds) {
    return 0.000172603109 * speed * seconds;
}

TEST_CASE("updateMileage") {
  const auto eps = 0.001;
  setup();

  SECTION("It accumulates miles at high speed") {
    timeTravel(0);
    updateMileage(0);
    REQUIRE(data.intervalCounter == 0.0);

    uint8_t speed = 100; // km/h
    uint16_t seconds = 60*60;
    for (uint32_t t = 0; t < seconds * 1000; t += 100) {
      timeTravel(t);
      updateMileage(speed);
    }

    double expectedDistance = distanceFromSpeed(speed, seconds);
    REQUIRE(data.intervalCounter == Approx(expectedDistance).epsilon(eps));
  }

  SECTION("It accumulates miles at low speed") {
    timeTravel(0);
    updateMileage(0);
    REQUIRE(data.intervalCounter == 0.0);

    uint8_t speed = 4; // km/h
    uint16_t seconds = 60*60;
    for (uint32_t t = 0; t < seconds * 1000; t += 100) {
      timeTravel(t);
      updateMileage(speed);
    }

    double expectedDistance = distanceFromSpeed(speed, seconds);
    REQUIRE(data.intervalCounter == Approx(expectedDistance).epsilon(eps));
  }
}

TEST_CASE("checkIntervalLimit") {
  const auto eps = 0.001;
  setup();

  SECTION("when below the limit") {
    data.intervalCounter = 3000;
    data.intervalLimit = 5000;
    checkIntervalLimit();
    REQUIRE(data.intervalCounter == Approx(3000).epsilon(eps));
    REQUIRE(data.intervalReached == 0);
  }

  SECTION("when above the limit") {
    data.intervalCounter = 5000.1;
    data.intervalLimit = 5000;
    checkIntervalLimit();
    REQUIRE(data.intervalCounter == Approx(0.1).epsilon(eps));
    REQUIRE(data.intervalReached == 1);
  }
}

CANMessage obdResponseForSpeed(uint8_t speed) {
  return CANMessage(0x7E8, { 0x03, 0x41, 0x0d, (int)speed, 0x00, 0x00, 0x00, 0x00 });
}

TEST_CASE("updateSpeed") {
  const auto eps = 0.001;
  setup();

  SECTION("OBD responds to speed request") {
    timeTravel(0);
    updateMileage(0);

    uint8_t speed = 100; // km/h
    uint16_t seconds = 60*60;
    for (uint32_t t = 0; t < seconds * 1000; t += 100) {
      timeTravel(t);

      // Must provide a CAN response ahead of time otherwise it leads to a timeout
      carloop.can().addRx(obdResponseForSpeed(100));
      updateSpeed();
    }

    double expectedDistance = distanceFromSpeed(speed, seconds);
    REQUIRE(data.intervalCounter == Approx(expectedDistance).epsilon(eps));
  }

  SECTION("OBD doesn't respond to speed request") {
    timeTravel(0);
    updateMileage(10);
    updateMileage(10);
    updateMileage(10);
    double previousMileage = data.intervalCounter;

    REQUIRE_FALSE(previousMileage == 0.0);

    // Causes a timeout since no OBD response was queued up ahead of time
    updateSpeed();

    REQUIRE(data.intervalCounter == previousMileage);
  }
}
