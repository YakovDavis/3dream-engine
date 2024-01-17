#include "SoundEngine.h"

#include "D3E/Debug.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/map.h"
#include "EASTL/string.h"
#include "SoundEngineCommon.h"
#include "utils/FilenameUtils.h"

#include <format>

using namespace D3E;
using eastl::map;

FMOD::Sound* SoundEngine::GetSound(const String& uuid)
{
	auto foundIt = sounds.find(uuid);

	if (foundIt == sounds.end())
		return nullptr;

	return foundIt->second;
}

SoundEngine& SoundEngine::GetInstance()
{
	static SoundEngine instance;

	return instance;
}

SoundEngine::SoundEngine() : driverCount{0}, system{nullptr}, initialized{false}
{
}

SoundEngine::~SoundEngine()
{
	Release();
}

void SoundEngine::Init()
{
	Debug::LogMessage("[FMOD] Init");

	CheckError(FMOD::System_Create(&system));
	CheckError(
		system->init(512, FMOD_INIT_NORMAL,
	                 nullptr)); // TODO: determine actual max channel number

	PrintDrivers();

	CheckError(system->set3DSettings(1.0, 1.0f, 1.0f));

	initialized = true;
}

void SoundEngine::PrintDrivers()
{
	if (CheckError(system->getNumDrivers(&driverCount)))
		return;

	for (auto i = 0; i < driverCount; ++i)
	{
		std::unique_ptr<char[]> name(new char[256]);

		system->getDriverInfo(i, name.get(), 256, nullptr, nullptr, nullptr,
		                      nullptr);
	}
}

void SoundEngine::Update()
{
	CheckError(system->update());
}

void SoundEngine::Release()
{
	if (!system)
		return;

	for (auto& s : sounds)
	{
		CheckError(s.second->release());
	}

	CheckError(system->close());
	CheckError(system->release());
}

bool SoundEngine::IsInitialized() const
{
	return initialized;
}

int SoundEngine::GetDriverCount() const
{
	return driverCount;
}

int SoundEngine::SetDriver(int id)
{
	if (!system)
		return 1;
	if (id < 0 || id > driverCount - 1)
		return 1;

	return CheckError(system->setDriver(id));
}

void SoundEngine::LoadSound(SoundMetaData& metadata, const std::string& directory)
{
	auto foundIt = sounds.find(metadata.uuid.c_str());
	if (foundIt != sounds.end())
		return;

	FMOD_MODE mode = FMOD_DEFAULT;
	mode |= metadata.is3d ? FMOD_3D : FMOD_2D;
	mode |= metadata.isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= metadata.stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* sound = nullptr;

	if (CheckError(system->createSound(FilenameUtils::MetaFilenameToFilePath(metadata.filename, directory).string().c_str(), mode, nullptr, &sound)))
		return;

	sounds[metadata.uuid.c_str()] = sound;
}

void SoundEngine::UnloadSound(const String& soundName)
{
	auto foundIt = sounds.find(soundName);

	if (foundIt == sounds.end())
		return;

	if (CheckError(foundIt->second->release()))
		return;

	sounds.erase(foundIt);
}

void SoundEngine::PlaySound3D(const String& soundName,
                              const DirectX::SimpleMath::Vector3 loc,
                              float dbVolume)
{
	auto* sound = GetSound(soundName);

	if (!sound)
		return;

	FMOD::Channel* channel = nullptr;
	if (CheckError(system->playSound(sound, nullptr, true, &channel)))
		return;

	if (!channel)
		return;

	FMOD_MODE soundMode{};

	if (CheckError(sound->getMode(&soundMode)))
		return;

	if (soundMode & FMOD_3D)
	{
		FMOD_VECTOR location = VectorToFmod(loc);
		FMOD_VECTOR vel = {0.f, 0.f, 0.f};

		if (CheckError(channel->set3DAttributes(&location, &vel)))
			return;
	}

	if (CheckError(channel->setVolume(DbToVolume(dbVolume))))
		return;

	CheckError(channel->setPaused(false));
}

void SoundEngine::PlaySound2D(const String& soundName, float dbVolume)
{
	const DirectX::SimpleMath::Vector3 v = {0.f, 0.f, 0.f};
	PlaySound3D(soundName, v, dbVolume);
}

void SoundEngine::SetChannelLocation(
	int channelId, const DirectX::SimpleMath::Vector3 loc)
{
}

void SoundEngine::SetChannelVolume(int channelId, float dbVolume)
{
	auto foundIt = channels.find(channelId);

	if (foundIt == channels.end())
		return;

	CheckError(foundIt->second->setVolume(DbToVolume(dbVolume)));
}

void D3E::SoundEngine::SetListenerTransform(
	DirectX::SimpleMath::Vector3 location,
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 forward,
	DirectX::SimpleMath::Vector3 upv)
{
	FMOD_VECTOR loc = VectorToFmod(location);
	FMOD_VECTOR vel = VectorToFmod(velocity);
	FMOD_VECTOR fwd = VectorToFmod(forward);
	FMOD_VECTOR up = VectorToFmod(upv);

	CheckError(system->set3DListenerAttributes(0, &loc, &vel, &fwd, &up));
}

bool SoundEngine::IsPlaying(int channelId) const
{
	return false;
}

bool SoundEngine::IsSoundUuidValid(const String& uuid) const
{
	return sounds.find(uuid) != sounds.end();
}
