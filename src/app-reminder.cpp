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

// OBD constants for CAN

// CAN IDs for OBD messages
const auto OBD_CAN_REQUEST_ID      = 0x7E0;
const auto OBD_CAN_REPLY_ID_MIN    = 0x7E8;

// Modes (aka services) for OBD
const auto OBD_MODE_CURRENT_DATA   = 0x01;

// OBD signals (aka PID) that can be requested
const auto OBD_PID_VEHICLE_SPEED   = 0x0d;

// Time to wait for a reply for an OBD request
const auto OBD_TIMEOUT_MS = 10;

uint8_t vehicleSpeedKmh = 0;

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

void setupCloud()
{
  Particle.function("reset", resetIntervalCounter);
  Particle.function("limit", changeIntervalLimit);
  Particle.variable("count", data.intervalCounter);
}

int resetIntervalCounter(String)
{
  data.intervalCounter = 0;
  data.intervalReached = 0;
  storage.store(data);
  return 0;
}

int changeIntervalLimit(String arg)
{
  long newLimit = arg.toInt();
  if (newLimit <= 0)
  {
    return -1;
  }

  data.intervalLimit = newLimit;
  while (data.intervalCounter >= data.intervalLimit)
  {
    data.intervalCounter -= data.intervalLimit;
    data.intervalReached = 1;
  }
  storage.store(data);
  return 0;
}

void loop()
{
  updateSpeed();
  publishEvent();
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
    data.intervalReached = 0;
    storage.store(data);
  }
}
