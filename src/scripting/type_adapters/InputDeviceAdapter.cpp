#include "InputDeviceAdapter.h"

#include "D3E/Debug.h"
#include "core/magic_enum.hpp"
#include "input/InputDevice.h"
#include "input/Keys.h"

#include <format>

using namespace D3E;

InputDeviceAdapter::InputDeviceAdapter(InputDevice* id) : inputDevice_(id)
{
}

bool InputDeviceAdapter::IsKeyDown(const std::string& key)
{
	auto k = magic_enum::enum_cast<Keys>(key.c_str());

	if (!k)
	{
		Debug::LogError(std::format("[InputDeviceAdapter] : IsKeyDown(): "
		                            "Failed to convert key {} to enum value.",
		                            key.c_str())
		                    .c_str());

		return false;
	}

	return inputDevice_->IsKeyDown(k.value());
}