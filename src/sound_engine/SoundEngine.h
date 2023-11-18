#pragma once

#include "EASTL/fixed_vector.h"
#include "EASTL/map.h"
#include "EASTL/string.h"
#include "SoundEngineCommon.h"
#include "fmod.h"
#include "fmod_studio.hpp"

namespace D3E
{
	class SoundEngine
	{
		typedef eastl::map<eastl::string, FMOD::Sound*> SoundMap;
		typedef eastl::map<int, FMOD::Channel*> ChannelMap;

	private:
		bool initialized;
		int driverCount;
		FMOD::System* system;

		SoundMap sounds;
		ChannelMap channels;

		SoundEngine();

		FMOD::Sound* GetSound(const eastl::string& name);

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

		void LoadSound(const eastl::string& path, bool is3d = false,
		               bool isLooping = false, bool stream = false);
		void UnloadSound(const eastl::string& soundName);
		void PlaySound3D(const eastl::string& soundName,
		                 const eastl::fixed_vector<float, 3, false> loc,
		                 float dbVolume = 1.0f);
		void PlaySound2D(const eastl::string& soundName, float dbVolume = 1.0f);
		void SetChannelLocation(int channelId,
		                        const eastl::fixed_vector<float, 3, false> loc);
		void SetChannelVolume(int channelId, float dbVolume);

		void SetListenerTransform(eastl::fixed_vector<float, 3, false> location,
		                         eastl::fixed_vector<float, 3, false> velocity,
		                         eastl::fixed_vector<float, 3, false> forward,
		                         eastl::fixed_vector<float, 3, false> up);

		bool IsPlaying(int channelId) const;
	};
} // namespace D3E