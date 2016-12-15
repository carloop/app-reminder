// Stub implementation of String
#pragma once
#include <string>

class String : public std::string {
  public:
    long toInt() {
      return atoi(c_str());
    }

    static String format(const char *format, const double &value) {
    }
};
