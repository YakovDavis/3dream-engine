#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "fmod.h"
#include "fmod_studio.hpp"
#include "SoundEngineCommon.h"

namespace SE {
using std::map;
using std::string;
using std::vector;

class SoundEngine {
  typedef map<string, FMOD::Sound*> SoundMap;
  typedef map<int, FMOD::Channel*> ChannelMap;

 private:
  bool initialized;
  int driverCount;
  FMOD::System* system;

  SoundMap sounds;
  ChannelMap channels;

  FMOD::Sound* GetSound(const std::string& name);

 public:
  SoundEngine();
  ~SoundEngine();

  void Init();
  void Update();
  void Release();

  bool IsInitialized() const;

  int GetDriverCount() const;
  int SetDriver(int id);

  void LoadSound(const string& path, bool is3d = false, bool isLooping = false,
                 bool stream = false);
  void UnloadSound(const string& soundName);
  void PlaySound(const string& soundName, const Vector3& loc,
                 float dbVolume = 1.0f);
  void PlaySound2D(const string& soundName, float dbVolume = 1.0f);
  void SetChannelLocation(int channelId, const Vector3& location);
  void SetChannelVolume(int channelId, float dbVolume);

  bool IsPlaying(int channelId) const;
};
}  // namespace SE