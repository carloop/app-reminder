// Test helpers to allow moving around in time during tests
#include "test_time.h"

unsigned long testMillis = 0;
unsigned long millis() {
  return testMillis++;
}

void delay(unsigned long t) {
}

void timeTravel(unsigned long newMillis) {
  testMillis = newMillis;
}
