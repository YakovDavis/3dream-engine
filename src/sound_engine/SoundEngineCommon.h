#pragma once

#include <iostream>

#include "fmod.hpp"
#include "fmod_errors.h"

namespace SE {
struct Vector3 {
  float x;
  float y;
  float z;
};

inline float DbToVolume(float dB) { return powf(10.0f, 0.05f * dB); }

inline float VolumeToDb(float volume) { return 20.0f * log10f(volume); }

inline FMOD_VECTOR VectorToFmod(const Vector3& pos) {
  return FMOD_VECTOR{pos.x, pos.y, pos.z};
}

inline int CheckError(FMOD_RESULT result) {
  if (result != FMOD_OK) {
    std::cout << FMOD_ErrorString(result) << std::endl;

    return 1;
  }

  return 0;
}

inline void Log(const std::string& level, const std::string& message) {
  std::cout << "Lvel: " << level << " msg: " << message << std::endl;
}
}  // namespace SE