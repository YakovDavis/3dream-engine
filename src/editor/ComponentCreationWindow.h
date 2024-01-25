#ifndef D3ENGINE_COMPONENTCREATIONWINDOW_H
#define D3ENGINE_COMPONENTCREATIONWINDOW_H

#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TPSControllerComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "entt/entt.hpp"
#include "imgui.h"

namespace D3E
{
	class Game;
	class Editor;

	class ComponentCreationWindow
	{
	private:
		Game* game_;
		Editor* editor_;
		FPSControllerComponent fpsControllerComponent;
		TPSControllerComponent tpsControllerComponent;
		PhysicsComponent physicsComponent;
		PhysicsCharacterComponent characterComponent;
		CameraComponent cameraComponent;
		LightComponent lightComponent;
		SoundComponent soundComponent;

	public:
		void Draw();
		explicit ComponentCreationWindow(Game* game, Editor* editor);
		bool open;
		int componentType;
		entt::entity currentEntity;
	};
}

#endif // D3ENGINE_COMPONENTCREATIONWINDOW_H
