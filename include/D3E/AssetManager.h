#pragma once

namespace D3E
{
	class AssetManager
	{
	public:
		static AssetManager& Get();



	private:
		AssetManager() = default;

		static AssetManager instance_;

	public:
		AssetManager(AssetManager const&) = delete;
		void operator=(AssetManager const&) = delete;
	};
}
