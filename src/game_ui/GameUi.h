#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Debug.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/string.h"
#include "render/Display.h"
#include "rmlui_backend/rmlui_backend.h"
#include "rmlui_backend/RmlUi_Platform_Win32.h"
#include "input/InputDevice.h"

namespace D3E
{
	class Game;

	class GameUi
	{
	private:
		static GameUi* instance_;

	public:
		static GameUi* Init(Game *game, nvrhi::IFramebuffer* fb);
		static GameUi* Get() { return instance_; }

		void PostInputInit();

		void Update();
		void Draw();

		static void DestroyResources();

	private:
		void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
		void OnKeyChanged(const InputDevice::KeyEventArgs& args);

		Game *game_;
		RmlUi_NVRHI rmlUiNvrhi_;
		SystemInterface_Win32 rmlUiWin32_;
		Rml::Context* context_;
		bool lmbPressedLastUpdate = false;

		struct ApplicationData {
			bool show_text = true;
			Rml::String animal = "dog";
		} my_data;

	private:
		GameUi(Game *game, nvrhi::IFramebuffer* fb);
	};
}
