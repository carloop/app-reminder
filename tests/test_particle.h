// Test stubs for Particle functions
#pragma once
#include "test_string.h"

enum PublishModeT {
  PUBLIC,
  PRIVATE
};

class ParticleClass {
public:
  using FunctionPtr = int (*)(String);
  
  void function(const char *name, const FunctionPtr ptr);
  void variable(const char *name, double &value);
  void publish(const char *event, String data, PublishModeT mode);
  bool connected();

  /* Test interface */
  void setConnected(bool value);

private:
  bool _connected;
};

extern ParticleClass Particle;
