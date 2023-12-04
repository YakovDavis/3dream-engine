#pragma once

#include "imgui.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"

namespace D3E
{
	class EditorConsole
	{
	private:
		bool open_;
		char inputBuffer_[256];
		eastl::vector<eastl::string> items_;
		ImVector<char*> history_;
		int historyPos_;
		ImGuiTextFilter filter_;
		bool autoScroll_;
		bool scrollToBottom_;
	private:
		int TextEditCallback(ImGuiInputTextCallbackData* data);
		static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	public:
		EditorConsole();
		void Draw();
		void PrintMessage(const eastl::string& str);
		void ClearLog();
	};
}

