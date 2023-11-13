#pragma once

#include "D3E/Debug.h"
#include "EASTL/fixed_vector.h"
#include "fmod.hpp"
#include "fmod_errors.h"

#include <iostream>

namespace D3E
{
	inline float DbToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	inline float VolumeToDb(float volume)
	{
		return 20.0f * log10f(volume);
	}

	inline FMOD_VECTOR
	VectorToFmod(const eastl::fixed_vector<float, 3, false>& pos)
	{
		return FMOD_VECTOR{pos[0], pos[1], pos[2]};
	}

	inline int CheckError(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			Debug::LogError(FMOD_ErrorString(result));

			return 1;
		}

		return 0;
	}
} // namespace D3E