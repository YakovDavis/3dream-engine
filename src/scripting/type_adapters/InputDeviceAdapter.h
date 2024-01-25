#pragma once

#include "D3E/CommonHeader.h"
#include "input/InputDevice.h"
#include "input/Keys.h"

namespace D3E
{
	class InputDeviceAdapter
	{
	public:
		explicit InputDeviceAdapter(InputDevice* id);

		bool IsKeyDown(const std::string& key);

	private:
		InputDevice* inputDevice_;
	};
} // namespace D3E