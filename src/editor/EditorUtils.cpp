#include "EditorUtils.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"

D3E::Game* D3E::EditorUtils::activeGame_ = nullptr;
bool D3E::EditorUtils::initialized_ = false;

void D3E::EditorUtils::Initialize(D3E::Game* game)
{
	activeGame_ = game;
	initialized_ = true;
}

eastl::vector<entt::entity> D3E::EditorUtils::ListActiveObjects()
{
	Debug::Assert(initialized_, "Editor utils not initialized");
	auto view = activeGame_->GetRegistry().view<ObjectInfoComponent>();
	eastl::vector<entt::entity> result;
	view.each([&result](const auto entity, const auto &info) {
				  result.push_back({entity});
	});
	return result;
}
