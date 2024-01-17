#pragma once

#include "D3E/CommonHeader.h"
#include "imgui.h"

namespace D3E
{
	class Editor;

	class MaterialEditor
	{
	private:
		Editor* editor_;

		std::string filename;

		std::string albedoInputField;
		std::string normalInputField;
		std::string metalnessInputField;
		std::string roughnessInputField;

		void SaveMaterial();

	public:
		void Draw();
		void OpenMaterial(const std::string& fname);
		explicit MaterialEditor(Editor* editor);
		bool open;
	};
}
