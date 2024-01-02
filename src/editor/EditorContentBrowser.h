#pragma once

#include "filesystem"
#include "imgui.h"
#include "string"

namespace D3E
{
	class EditorContentBrowser
	{
	private:
		bool open_;
		std::filesystem::path rootDirectory_;
		std::filesystem::path currentDirectory_;
		static std::string RemoveExtension(std::string basicString);
		static std::string RemovePath(std::string str);
	public:
		void Draw();
		EditorContentBrowser();
	};
}

