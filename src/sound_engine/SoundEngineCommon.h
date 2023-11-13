#pragma once

#include <iostream>

#include "fmod.hpp"
#include "fmod_errors.h"
#include "D3E/Debug.h"

namespace D3E {
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
	  Debug::LogError(FMOD_ErrorString(result));
    
    return 1;
  }

  return 0;
}
}  // namespace SE