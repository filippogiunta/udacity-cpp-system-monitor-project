#include "format.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using std::string;

// TODO: Complete this helper function
string Format::ElapsedTime(long seconds) {
  auto hours = seconds / 3600;
  auto minutes = (seconds / 60) % 60;
  auto secs = seconds % 60;

  std::ostringstream timeStream;
  timeStream << std::setw(2) << std::setfill('0') << hours << ":"
             << std::setw(2) << std::setfill('0') << minutes << ":"
             << std::setw(2) << std::setfill('0') << secs;

  return timeStream.str();
}