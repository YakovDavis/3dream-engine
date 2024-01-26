#include "GameUi.h"

#include "D3E/Game.h"
#include "RmlUi/Core.h"
#include "core/EngineState.h"
#include "render/DisplayWin32.h"

D3E::GameUi* D3E::GameUi::instance_ = nullptr;

D3E::GameUi* D3E::GameUi::Init(D3E::Game* game, nvrhi::IFramebuffer* fb)
{
	if (instance_ == nullptr)
	{
		instance_ = new GameUi(game, fb);
		Debug::LogMessage("[RmlUi] Init");
	}

	return instance_;
}

D3E::GameUi::GameUi(D3E::Game* game, nvrhi::IFramebuffer* fb) : game_(game), rmlUiWin32_(), rmlUiNvrhi_(game, fb)
{
	rmlUiWin32_.SetWindow(game_->GetDisplayWin32()->hWnd);
	Rml::SetSystemInterface(&rmlUiWin32_);
	Rml::SetRenderInterface(&rmlUiNvrhi_);
}

void D3E::GameUi::DestroyResources()
{
}

void D3E::GameUi::Update()
{
	/*if (!lmbPressedLastUpdate && game_->GetInputDevice()->IsKeyDown(Keys::LeftButton))
	{
		context_->ProcessMouseButtonDown(0, 0);
	}
	else if (lmbPressedLastUpdate && !game_->GetInputDevice()->IsKeyDown(Keys::LeftButton))
	{
		context_->ProcessMouseButtonDown(1, 0);
	}*/
	lmbPressedLastUpdate = game_->GetInputDevice()->IsKeyDown(Keys::LeftButton);
}

void D3E::GameUi::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	//context_->ProcessMouseMove(game_->GetInputDevice()->MousePosition.x, game_->GetInputDevice()->MousePosition.y, 0);
}

void D3E::GameUi::PostInputInit()
{
	game_->GetInputDevice()->MouseMove.AddRaw(this, &GameUi::OnMouseMove);
	game_->GetInputDevice()->KeyChanged.AddRaw(this, &GameUi::OnKeyChanged);
}

void D3E::GameUi::OnKeyChanged(const D3E::InputDevice::KeyEventArgs& args)
{
	/*if (args.down)
	{
		switch (args.key)
		{
			default:
				context_->ProcessKeyDown((Input::KeyIdentifier)args.key, 0);
				return;
		}
	}
	else
	{
		switch (args.key)
		{
			default:
				context_->ProcessKeyUp((Input::KeyIdentifier)args.key, 0);
				return;
		}
	}*/
}

void D3E::GameUi::GameEnd()
{
	Rml::Shutdown();
}

void D3E::GameUi::GameStart()
{
	Rml::SetSystemInterface(&rmlUiWin32_);
	Rml::SetRenderInterface(&rmlUiNvrhi_);
	Rml::Initialise();
}
