#pragma once

#include "imgui.h"
#include "string"
#include "filesystem"

namespace D3E
{
	class EditorContentBrowser
	{
	private:
		bool open_;
		std::filesystem::path rootDirectory_;
		std::filesystem::path currentDirectory_;
	public:
		void Draw();
		EditorContentBrowser();
	};
}

