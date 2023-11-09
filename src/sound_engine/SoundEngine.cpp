#include "SoundEngine.h"

#include "SoundEngineCommon.h"

using namespace SE;

FMOD::Sound* SoundEngine::GetSound(const std::string& name) {
  auto foundIt = sounds.find(name);

  if (foundIt == sounds.end()) return nullptr;

  return foundIt->second;
}

SoundEngine::SoundEngine()
    : driverCount{0}, system{nullptr}, initialized{false} {}

SoundEngine::~SoundEngine() { Release(); }

void SoundEngine::Init() {
  CheckError(FMOD::System_Create(&system));
  CheckError(
      system->init(512, FMOD_INIT_NORMAL,
                   nullptr));  // TODO: determine actual max channel number

  system->getNumDrivers(&driverCount);

  for (auto i = 0; i < driverCount; ++i) {
    std::unique_ptr<char[]> name(new char[256]);

    system->getDriverInfo(i, name.get(), 256, nullptr, nullptr, nullptr,
                          nullptr);
    std::cout << "Driver id: " << i << " Driver name: " << name << std::endl;
  }

  initialized = true;
}

void SoundEngine::Update() { CheckError(system->update()); }

void SoundEngine::Release() {
  if (!system) return;

  for (auto& s : sounds) {
    CheckError(s.second->release());
  }

  CheckError(system->close());
  CheckError(system->release());
}

bool SoundEngine::IsInitialized() const { return initialized; }

int SoundEngine::GetDriverCount() const { return driverCount; }

int SoundEngine::SetDriver(int id) {
  if (!system) return 1;
  if (id < 0 || id > driverCount - 1) return 1;

  return CheckError(system->setDriver(id));
}

void SoundEngine::LoadSound(const string& path, bool is3d, bool isLooping,
                            bool stream) {
  auto foundIt = sounds.find(path);
  if (foundIt != sounds.end()) return;

  FMOD_MODE mode = FMOD_DEFAULT;
  mode |= is3d ? FMOD_3D : FMOD_2D;
  mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
  mode |= stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

  FMOD::Sound* sound = nullptr;

  if (CheckError(system->createSound(path.c_str(), mode, nullptr, &sound)))
    return;

  sounds[path] = sound;
}

void SoundEngine::UnloadSound(const string& soundName) {
  auto foundIt = sounds.find(soundName);

  if (foundIt == sounds.end()) return;

  if (CheckError(foundIt->second->release())) return;

  sounds.erase(foundIt);
}

void SoundEngine::PlaySound(const string& soundName, const Vector3& loc,
                            float dbVolume) {
  auto* sound = GetSound(soundName);

  if (!sound) return;

  FMOD::Channel* channel = nullptr;
  if (CheckError(system->playSound(sound, nullptr, true, &channel))) return;

  if (!channel) return;

  FMOD_MODE soundMode{};

  if (CheckError(sound->getMode(&soundMode))) return;

  if (soundMode & FMOD_3D) {
    FMOD_VECTOR location = VectorToFmod(loc);
    if (CheckError(channel->set3DAttributes(&location, nullptr))) return;
  }

  if (CheckError(channel->setVolume(DbToVolume(dbVolume)))) return;
  if (CheckError(channel->setPaused(false))) return;
}

void SoundEngine::PlaySound2D(const string& soundName, float dbVolume) {
  PlaySound(soundName, Vector3(0.f, 0.f, 0.f), dbVolume);
}

void SoundEngine::SetChannelLocation(int channelId, const Vector3& location) {}

void SoundEngine::SetChannelVolume(int channelId, float dbVolume) {}

bool SoundEngine::IsPlaying(int channelId) const { return false; }
