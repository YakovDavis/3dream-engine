#include "InputDevice.h"

#include "D3E/CommonCpp.h"
#include <iostream>
#include "D3E/Game.h"
#include "render/DisplayWin32.h"

using namespace DirectX::SimpleMath;

D3E::InputDevice::InputDevice(Game* inGame) : game(inGame)
{
	keys = new eastl::unordered_set<Keys>();
	
	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = game->GetDisplayWin32()->hWnd;

	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0;   // adds HID keyboard and also ignores legacy keyboard messages
	Rid[1].hwndTarget = game->GetDisplayWin32()->hWnd;

	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
	{
		auto errorCode = GetLastError();
		std::cout << "ERROR: " << errorCode << std::endl;
	}
}

D3E::InputDevice::~InputDevice()
{
	delete keys;
}

void D3E::InputDevice::OnKeyDown(KeyboardInputEventArgs args)
{
	bool Break = args.Flags & 0x01;

	auto key = static_cast<Keys>(args.VKey);

	if (args.MakeCode == 42) key = Keys::LeftShift;
	if (args.MakeCode == 54) key = Keys::RightShift;
	
	if(Break) {
		if(keys->count(key))	RemovePressedKey(key);
	} else {
		if (!keys->count(key))	AddPressedKey(key);
	}
}

void D3E::InputDevice::OnMouseMove(RawMouseEventArgs args)
{
	if(args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonDown))
		AddPressedKey(Keys::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonUp))
		RemovePressedKey(Keys::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonDown))
		AddPressedKey(Keys::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonUp))
		RemovePressedKey(Keys::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonDown))
		AddPressedKey(Keys::MiddleButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonUp))
		RemovePressedKey(Keys::MiddleButton);

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(game->GetDisplayWin32()->hWnd, &p);
	
	MousePosition = Vector2(static_cast<float>(p.x), static_cast<float>(p.y));
	MouseOffset = Vector2(static_cast<float>(args.X), static_cast<float>(args.Y));
	MouseOffsetInTick += MouseOffset;
	MouseWheelDelta = args.WheelDelta;

	const MouseMoveEventArgs moveArgs = {MousePosition, MouseOffset, MouseWheelDelta};
	
	MouseMove.Broadcast(moveArgs);
}

void D3E::InputDevice::AddPressedKey(Keys key)
{
	//if (!game->isActive) {
	//	return;
	//}
	keys->insert(key);
}

void D3E::InputDevice::RemovePressedKey(Keys key)
{
	keys->erase(key);
}

bool D3E::InputDevice::IsKeyDown(Keys key)
{
	return keys->count(key);
}

void D3E::InputDevice::EndTick()
{
	MouseOffsetInTick.x = 0;
	MouseOffsetInTick.y = 0;
}
