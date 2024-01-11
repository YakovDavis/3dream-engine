#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/map.h"
#include "EASTL/string.h"
#include "SimpleMath.h"
#include "SoundEngineCommon.h"
#include "assetmng/SoundMetaData.h"
#include "fmod.h"
#include "fmod_studio.hpp"

namespace D3E
{
	class SoundEngine
	{
		typedef eastl::map<String, FMOD::Sound*> SoundMap;
		typedef eastl::map<int, FMOD::Channel*> ChannelMap;

	private:
		bool initialized;
		int driverCount;
		FMOD::System* system;

		SoundMap sounds;
		ChannelMap channels;

		SoundEngine();

		FMOD::Sound* GetSound(const eastl::string& uuid);

	public:
		static SoundEngine& GetInstance();

		SoundEngine(SoundEngine const&) = delete;
		SoundEngine& operator=(SoundEngine const&) = delete;

		~SoundEngine();

		void Init();
		void PrintDrivers();
		void Update();
		void Release();

		bool IsInitialized() const;

		int GetDriverCount() const;
		int SetDriver(int id);

		bool IsSoundUuidValid(const String& uuid) const;

		void LoadSound(SoundMetaData& metadata);
		void UnloadSound(const String& uuid);
		void PlaySound3D(const eastl::string& uuid,
		                 const DirectX::SimpleMath::Vector3 loc,
		                 float dbVolume = 1.0f);
		void PlaySound2D(const eastl::string& uuid, float dbVolume = 1.0f);
		void SetChannelLocation(int channelId,
		                        const DirectX::SimpleMath::Vector3 loc);
		void SetChannelVolume(int channelId, float dbVolume);

		void SetListenerTransform(DirectX::SimpleMath::Vector3 location,
		                          DirectX::SimpleMath::Vector3 velocity,
		                          DirectX::SimpleMath::Vector3 forward,
		                          DirectX::SimpleMath::Vector3 up);

		bool IsPlaying(int channelId) const;
	};
} // namespace D3E