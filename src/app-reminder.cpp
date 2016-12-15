/* Carloop mileage reminder app
 * Copyright 2016 1000 Tools, Inc.
 *
 * Accumulates miles in a counter stored in EEPROM.
 * When the limit is reached, set a flag in EEPROM that an event should
 * be published to the Particle Cloud.
 * In the cloud, set the event to send an SMS to the driver.
 */
#include "application.h"
#include "carloop/carloop.h"
#include "storage.h"

// Function declarations
void publishEvent();
void setupCloud();
int resetIntervalCounter(String);
int changeIntervalLimit(String arg);
void updateSpeed();
void requestVehicleSpeed();
void waitForVehicleSpeedResponse();
void updateMileage(uint8_t newVehicleSpeedKhm);
double computeDeltaMileage(uint8_t newVehicleSpeedKhm);
void checkIntervalLimit();
void storeMileage();

// Don't block the main program while connecting to WiFi/cellular.
// This way the main program runs on the Carloop even outside of WiFi range.
SYSTEM_THREAD(ENABLED);

// Tell the program which revision of Carloop you are using.
Carloop<CarloopRevision2> carloop;

// The data that is stored and loaded in permanent storage (EEPROM)
// including the current interval count and limit
Data data;
// The class that will read and write the data to EEPROM
Storage storage;
// Only store to EEPROM every so often
const auto STORAGE_PERIOD = 60 * 1000; /* every minute */
uint32_t lastStorageTime = 0;

// OBD constants for CAN

// CAN IDs for OBD messages
const auto OBD_CAN_REQUEST_ID    = 0x7E0;
const auto OBD_CAN_REPLY_ID      = 0x7E8;

// Modes (aka services) for OBD
const auto OBD_MODE_CURRENT_DATA = 0x01;

// OBD signals (aka PID) that can be requested
const auto OBD_PID_VEHICLE_SPEED = 0x0d;

// Time to wait for a reply for an OBD request
const auto OBD_TIMEOUT_MS = 10;

uint8_t vehicleSpeedKmh = 0;
uint32_t lastVehicleSpeedUpdateTime = 0;

// Called at boot
// Sets up the CAN bus and cloud functions
void setup()
{
  setupCloud();
  storage.load(data);

  // Configure the CAN bus speed for 500 kbps, the standard speed for the OBD-II port.
  // Other common speeds are 250 kbps and 1 Mbps.
  carloop.setCANSpeed(500000);

  // Connect to the CAN bus
  carloop.begin();
}

// Allow interacting with the Carloop remotely
void setupCloud()
{
  Particle.function("reset", resetIntervalCounter);
  Particle.function("limit", changeIntervalLimit);
  Particle.variable("count", data.intervalCounter);
}

// Reset the interval counter and store the zero value in EEPROM
int resetIntervalCounter(String)
{
  data.intervalCounter = 0;
  data.intervalReached = 0;
  storage.store(data);
  return 0;
}

// Set the interval upper limit and make sure the current value is below
// that. Store the values value in EEPROM
int changeIntervalLimit(String arg)
{
  long newLimit = arg.toInt();
  if (newLimit <= 0)
  {
    return -1;
  }

  data.intervalLimit = newLimit;
  checkIntervalLimit();
  storage.store(data);
  return 0;
}

// Called over and over
// Process new CAN messages here to update the vehicle speed, update
// the mileage and update the interval counter
void loop()
{
  updateSpeed();
  storeMileage();
  publishEvent();
  delay(100);
}

void updateSpeed()
{
  requestVehicleSpeed();
  waitForVehicleSpeedResponse();
}

void requestVehicleSpeed()
{
  CANMessage message;

  // A CAN message to request the vehicle speed
  message.id = OBD_CAN_REQUEST_ID;
  message.len = 8;

  // Data is an OBD request: get current value of the vehicle speed PID
  message.data[0] = 2; // 2 byte request
  message.data[1] = OBD_MODE_CURRENT_DATA;
  message.data[2] = OBD_PID_VEHICLE_SPEED;

  // Send the message on the bus!
  carloop.can().transmit(message);
}

void waitForVehicleSpeedResponse()
{
  uint32_t start = millis();
  bool received = false;
  while (millis() - start < OBD_TIMEOUT_MS)
  {
    CANMessage message;
    if (carloop.can().receive(message))
    {
      if (message.id == OBD_CAN_REPLY_ID &&
          message.data[2] == OBD_PID_VEHICLE_SPEED)
      {
        received = true;
        uint8_t newVehicleSpeedKhm = message.data[3];
        updateMileage(newVehicleSpeedKhm);
      }
    }
  }

  if (!received)
  {
    updateMileage(0);
  }
}

void updateMileage(uint8_t newVehicleSpeedKhm)
{
  double deltaMileage = computeDeltaMileage(newVehicleSpeedKhm);

  data.intervalCounter += deltaMileage;
  checkIntervalLimit();
}

double computeDeltaMileage(uint8_t newVehicleSpeedKhm)
{
  uint32_t now = millis();
  double deltaMileage = 0.0;

  // If the speed was previously 0 or newly 0, or timed out because the
  // car was off, just save the new speed value
  if (vehicleSpeedKmh > 0 && newVehicleSpeedKhm > 0)
  {
    // The car was previously driving and is still driving

    // Figure out the distance driven using the trapezoidal rule
    uint32_t msDiff = now - lastVehicleSpeedUpdateTime;
    // Calculate only if the difference is plausible
    if (msDiff < 1000) {
      // distance in km/h * ms
      uint32_t deltaDistance = msDiff * (vehicleSpeedKmh + newVehicleSpeedKhm) / 2;

      // Convert to miles
      // 1 kilometer per hour * ms = 1.72603109 × 10^-7 miles
      // km/h * ms * (1 h / 3600 s) * (0.621371 mi / 1 km) * (1 s / 1000 ms)
      deltaMileage = deltaDistance * 1.72603109e-7;
    }
  }

  vehicleSpeedKmh = newVehicleSpeedKhm;
  lastVehicleSpeedUpdateTime = now;
  return deltaMileage;
}

// If the interval limit is reached, mark it so we can publish an event
// when we come back to network range
void checkIntervalLimit()
{
  while (data.intervalCounter >= data.intervalLimit)
  {
    data.intervalCounter -= data.intervalLimit;
    data.intervalReached = 1;
  }
}

// Store data to EEPROM every so often
void storeMileage()
{
  if (millis() - lastStorageTime > STORAGE_PERIOD)
  {
    storage.store(data);
    lastStorageTime = millis();
  }
}

// If we reached the limit and we're connected to the cloud through WiFi
// or cellular, publish the event
void publishEvent()
{
  if (data.intervalReached && Particle.connected())
  {
    // Publish the event interval with the value set to the mileage that was reached
    Particle.publish("interval", String::format("%.0f", data.intervalLimit), PRIVATE);
    // Clear the flag avoid publishing the same event twice
    data.intervalReached = 0;
    storage.store(data);
  }
}
