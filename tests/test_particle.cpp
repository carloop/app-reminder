// Test stubs for Particle functions
#include "test_particle.h"

void ParticleClass::function(const char *name, const ParticleClass::FunctionPtr ptr) {
}

void ParticleClass::variable(const char *name, float &value) {
}

void ParticleClass::publish(const char *event, String data, PublishModeT mode) {
}

bool ParticleClass::connected() {
  return _connected;
}

void ParticleClass::setConnected(bool value) {
  _connected = value;
}

ParticleClass Particle;
