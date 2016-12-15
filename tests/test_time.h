// Test helpers to allow moving around in time during tests
#pragma once

unsigned long millis();
void delay(unsigned long t);
void timeTravel(unsigned long newMillis);
